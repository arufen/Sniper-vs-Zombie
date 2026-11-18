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
#include <System/Component/ComponentCamera.h>
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>

namespace SniperVsZombie {
const float3 CAMERA_SPAWN_POSITION = float3{359.585f, 146.337f, 88.709f};

namespace {
//settings panel layout, 1280x720 screen(設定パネルの配置。1280x720画面)
constexpr int   SETTINGS_PANEL_LEFT   = 340;
constexpr int   SETTINGS_PANEL_TOP    = 180;
constexpr int   SETTINGS_PANEL_RIGHT  = 940;
constexpr int   SETTINGS_PANEL_BOTTOM = 510;
const Float2    SETTINGS_SLIDER_POS   = {400.0f, 320.0f};    //left end of the bar, y = its middle(バーの左端、yは中央)
constexpr float SETTINGS_SLIDER_WIDTH = 480.0f;
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
    Sound::PlayLoop(bgm_, Config::BGM_VOLUME);

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
    auto sliderObj = Scene::Object::Create<Object>();
    sliderObj->SetName("SensitivitySlider");
    sensitivitySlider_ = sliderObj->AddComponent<ComponentSlider>();
    sensitivitySlider_
        ->Setup(SETTINGS_SLIDER_POS, SETTINGS_SLIDER_WIDTH, Settings::MOUSE_SENSITIVITY_MIN, Settings::MOUSE_SENSITIVITY_MAX, Settings::GetMouseSensitivity())
        ->SetStep(Settings::MOUSE_SENSITIVITY_STEP)
        ->SetOnChanged([](float value) { Settings::SetMouseSensitivity(value); });

    //BACK is far from SETTINGS on purpose: the click that opens the panel can't also press BACK(BACKはわざとSETTINGSから離す: 開いたクリックでBACKが押されないように)
    auto backButtonObj = Scene::Object::Create<Object>();
    backButtonObj->SetName("BackButton");
    backButton_ = backButtonObj->AddComponent<ComponentButton>();
    backButton_->Setup({540.0f, 420.0f}, {200.0f, 60.0f}, "BACK", GetColor(0, 200, 0), [this]() { SetSettingsOpen(false); });

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
    sensitivitySlider_->SetStatus(Component::StatusBit::Enable, open);
    backButton_->SetStatus(Component::StatusBit::Enable, open);

    if(open)
        sensitivitySlider_->SetValue(Settings::GetMouseSensitivity());
}

void SniperVsZombie_MainMenu::DrawSettingsPanel()
{
    //dim the menu behind the panel(パネルの後ろのメニューを暗くする)
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
    DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    //panel box(パネルの箱)
    DrawBox(SETTINGS_PANEL_LEFT, SETTINGS_PANEL_TOP, SETTINGS_PANEL_RIGHT, SETTINGS_PANEL_BOTTOM, GetColor(25, 25, 30), TRUE);
    DrawBox(SETTINGS_PANEL_LEFT, SETTINGS_PANEL_TOP, SETTINGS_PANEL_RIGHT, SETTINGS_PANEL_BOTTOM, GetColor(0, 200, 0), FALSE);

    const unsigned int white = GetColor(255, 255, 255);
    const unsigned int gray  = GetColor(170, 170, 170);
    const int          mid_x = (SETTINGS_PANEL_LEFT + SETTINGS_PANEL_RIGHT) / 2;

    Hud::DrawLabelCenter(mid_x, SETTINGS_PANEL_TOP + 20, white, "設定");    //EN: Settings

    const int label_y = static_cast<int>(SETTINGS_SLIDER_POS.y) - 55;
    Hud::DrawLabel(static_cast<int>(SETTINGS_SLIDER_POS.x), label_y, white, "マウス感度");    //EN: Mouse sensitivity
    Hud::DrawLabelRight(static_cast<int>(SETTINGS_SLIDER_POS.x + SETTINGS_SLIDER_WIDTH), label_y, white, "%.2fx", sensitivitySlider_->GetValue());

    sensitivitySlider_->DrawSlider();

    Hud::DrawLabelCenter(
        mid_x, static_cast<int>(SETTINGS_SLIDER_POS.y) + 25, gray, "スコープ中は自動で遅くなります");    //EN: It slows down automatically while scoped

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
