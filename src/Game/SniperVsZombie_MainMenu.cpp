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
#include <System/Component/ComponentCamera.h>
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>

namespace SniperVsZombie {
const float3 CAMERA_SPAWN_POSITION = float3{359.585f, 146.337f, 88.709f};
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

    auto exitButtonObj = Scene::Object::Create<Object>();
    exitButtonObj->SetName("ExitButton");
    exitButton_ = exitButtonObj->AddComponent<ComponentButton>();
    exitButton_->Setup({66.0f, 480.0f}, {200.0f, 60.0f}, "EXIT", GetColor(200, 0, 0), []() { ::PostMessage(DxLib::GetMainWindowHandle(), WM_CLOSE, 0, 0); });

    return true;
}

void SniperVsZombie_MainMenu::Update()
{
    __super::Update();
}

void SniperVsZombie_MainMenu::PostDraw()
{
    __super::PostDraw();
    DrawGraph(0, 0, titleHandle, TRUE);

    //BUTTONS
    playButton_->DrawButton();
    exitButton_->DrawButton();

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades
    blackScreen_->DrawBlackScreen();
}

}    // namespace SniperVsZombie
