//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_MainMenu.cpp
//! @brief	SniperVsZombie_MainMenu
//---------------------------------------------------------------------------
#include "SniperVsZombie_MainMenu.h"
#include "ObjectNames.h"
#include "ShaderCamera.h"
#include "CameraProbe.h"
#include "StageConfig.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_Sky.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_MainStage.h"
#include "Hit.h"
#include "SoundUtil.h"
#include "AssetPaths.h"
#include "GameSettings.h"
#include "HudText.h"
#include "SettingsSliders.h"
#include <System/Component/ComponentCamera.h>
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>

namespace SniperVsZombie {
const float3 CAMERA_SPAWN_POSITION = float3{359.585f, 146.337f, 88.709f};

namespace {
//settings panel layout, 1280x720 screen(設定パネルの配置。1280x720画面)
//the panel box and sliders live in SettingsSliders.h (shared with the pause menu)(パネルの箱とスライダーはSettingsSliders.h。ポーズメニューと共有)
}    // namespace
//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_MainMenu::Init()
{
    //CAMERA
    auto camera     = Scene::Object::Create<Object>();
    auto cameraComp = camera->AddComponent<ComponentCamera>();
    cameraComp->SetPositionAndTarget({66.70f, -11.70f, -57.40f}, {0.0f, 0.0f, 1.0f});
    cameraComp->SetPerspective(60.0f);
    camera->SetTranslate(CAMERA_SPAWN_POSITION);
    camera->SetName(Names::CAMERA);

    //MAP - same positions as MainStage, from StageConfig.h(マップ。MainStageと同じ位置をStageConfig.hから)
    Scene::Object::Create<City>();
    auto woodTower = Scene::Object::Create<WoodTower>();
    woodTower->SetTranslate(Config::WOOD_TOWER_POSITION);
    woodTower->SetRotationAxisXYZ(Config::WOOD_TOWER_ROTATION);

    //Sky
    Scene::Object::Create<Sky>();
    Scene::Object::Create<ShaderCamera>();    //sends the real camera position to ps_model.fx(本当のカメラ位置をps_model.fxへ送る)
    CreateCameraProbe();                      //dev tool: F7 show, F8 copy camera position. Debug only(開発ツール: F7表示、F8でカメラ位置をコピー。Debugのみ)

    //Image
    titleHandle = LoadGraph("data/Game/UI/title.png");

    //BGM - silent until the file exists (RequiredAssets.md)(ファイルができるまで無音)
    bgm_ = Sound::Load(Assets::BGM_MENU);
    Sound::PlayBgm(bgm_, Config::BGM_VOLUME);

    //BLACK SCREEN TRANSITION
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName(Names::BLACK_SCREEN);
    blackScreen_ = blackScreenObj->AddComponent<ComponentBlackScreen>();
    blackScreen_->StartFadeIn();    // start fully black, fade INTO the menu

    //BUTTONS
    auto playButtonObj = Scene::Object::Create<Object>();
    playButtonObj->SetName("PlayButton");
    playButton_ = playButtonObj->AddComponent<ComponentButton>();
    playButton_->Setup({66.0f, 400.0f}, {200.0f, 60.0f}, "PLAY", GetColor(0, 200, 0), [this]() {
        // fade to black first, THEN change scene once the screen is fully black
        blackScreen_->StartFadeOut([]() { Scene::Change(Scene::GetScene<SniperVsZombie_MainStage>()); });
    });

    auto settingsButtonObj = Scene::Object::Create<Object>();
    settingsButtonObj->SetName("SettingsButton");
    settingsButton_ = settingsButtonObj->AddComponent<ComponentButton>();
    settingsButton_->Setup({66.0f, 480.0f}, {200.0f, 60.0f}, "SETTINGS", GetColor(60, 60, 160), [this]() { SetSettingsOpen(true); });

    auto exitButtonObj = Scene::Object::Create<Object>();
    exitButtonObj->SetName("ExitButton");
    exitButton_ = exitButtonObj->AddComponent<ComponentButton>();
    exitButton_->Setup({66.0f, 560.0f}, {200.0f, 60.0f}, "EXIT", GetColor(200, 0, 0), []() { ::PostMessage(DxLib::GetMainWindowHandle(), WM_CLOSE, 0, 0); });

    //SETTINGS PANEL - hidden until SETTINGS is clicked(設定パネル。SETTINGSを押すまで隠す)
    //BGM changes are heard right away while dragging(BGMの変更はドラッグ中にすぐ聞こえる)
    settingsSliders_.Create([this](SettingsSliders::Kind kind, float) {
        if(kind == SettingsSliders::Kind::Bgm)
            Sound::PlayBgm(bgm_, Config::BGM_VOLUME);
    });

    //BACK is far from SETTINGS on purpose: the click that opens the panel can't also press BACK(BACKはわざとSETTINGSから離す: 開いたクリックでBACKが押されないように)
    auto backButtonObj = Scene::Object::Create<Object>();
    backButtonObj->SetName("BackButton");
    backButton_ = backButtonObj->AddComponent<ComponentButton>();
    backButton_->Setup({Panel::CENTER_X - 100.0f, Panel::BUTTON_Y}, {200.0f, 60.0f}, "BACK", GetColor(0, 200, 0), [this]() { SetSettingsOpen(false); });

    SetSettingsOpen(false);    //the scene object is reused, so always start closed(シーンは使い回されるので、必ず閉じた状態で始める)

    return true;
}

void SniperVsZombie_MainMenu::Update()
{
    __super::Update();
}

void SniperVsZombie_MainMenu::SetSettingsOpen(bool open)
{
    //closing = the player is done, keep the value for next time(閉じる=調整完了。次回のために保存)
    if(settingsOpen_ && !open)
        Settings::Save();
    settingsOpen_ = open;

    //Enable = false makes the scene skip the component's Update, so hidden widgets can't be clicked(Enable=falseでシーンがUpdateを飛ばすので、隠れた部品は押せない)
    playButton_->SetStatus(Component::StatusBit::Enable, !open);
    settingsButton_->SetStatus(Component::StatusBit::Enable, !open);
    exitButton_->SetStatus(Component::StatusBit::Enable, !open);
    settingsSliders_.SetVisible(open);
    backButton_->SetStatus(Component::StatusBit::Enable, open);

    if(open)
        settingsSliders_.SyncFromSettings();
}

void SniperVsZombie_MainMenu::DrawSettingsPanel()
{
    SettingsSliders::DrawPanel("設定");    //EN: Settings
    settingsSliders_.DrawSliders();
    backButton_->DrawButton();
}

void SniperVsZombie_MainMenu::PostDraw()
{
    __super::PostDraw();
    DrawGraph(0, 0, titleHandle, TRUE);

    //BUTTONS
    playButton_->DrawButton();
    settingsButton_->DrawButton();
    exitButton_->DrawButton();

    if(settingsOpen_)
        DrawSettingsPanel();

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades
    blackScreen_->DrawBlackScreen();
}

void SniperVsZombie_MainMenu::Exit()
{
    Sound::Release(bgm_);
    Hud::ReleaseFont();
}

}    // namespace SniperVsZombie
