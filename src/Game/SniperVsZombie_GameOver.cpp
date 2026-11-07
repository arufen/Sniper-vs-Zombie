//---------------------------------------------------------------------------
//!	@file	Tutorial_X.cpp
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include "SniperVsZombie_GameOver.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_Sky.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_Helicopter.h"
#include "SniperVsZombie_Enemy.h"
#include "SniperVsZombie_MainStage.h"
#include "SniperVsZombie_MainMenu.h"
#include "Component/State/ComponentStateWalk.h"
#include "SniperVsZombie_Gate.h"
#include "Hit.h"
#include <System/Component/ComponentCamera.h>
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>

namespace SniperVsZombie {
const float3 CAMERA_SPAWN_POSITION = float3{359.585f, 146.337f, 88.709f};
//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_GameOver::Init()
{
    //CAMERA
    //Scene::Object::Create<Camera>()->SetTranslate(CAMERA_SPAWN_POSITION);
    auto camera     = Scene::Object::Create<Object>();
    auto cameraComp = camera->AddComponent<ComponentCamera>();
    camera->SetName("Camera");

    //MAP
    Scene::Object::Create<City>();
    auto   woodTower     = Scene::Object::Create<WoodTower>();
    float3 watchTowerPos = {437.0f, 100.0f, 550.0f};
    woodTower->SetTranslate({watchTowerPos});
    woodTower->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});

    //Sky
    Scene::Object::Create<Sky>();

    //Load helicopter
    auto heli = Scene::Object::Create<Helicopter>();
    heli->SetTranslate({1.360f, 414.269f, 700.566f});
    heli->SetScaleAxisXYZ({0.75f, 0.75f, 0.75f});
    heli->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});
    helicopter_ = heli;    // remember for cutscene animation

    //load gate
    //GATE
    auto gate = Scene::Object::Create<Gate>();
    gate->SetName("Gate_cutscene");
    gate->RemoveComponent<ComponentCollisionModel>();    // don't need collision for the cutscene
    float3 gatePos   = {67.726, 0.911, 760.512};
    float3 gateScale = {0.75f, 0.75f, 0.75f};
    gate->SetTranslate(gatePos);
    gate->SetScaleAxisXYZ(gateScale);
    gate->SetRotationAxisXYZ({0.0f, 90.0f, 0.0f});

    //CUTSCENE - isWin_ is already set (MainStage calls SetResult() right before Scene::Change)
    if(isWin_) {
        camera->SetTranslate({122.768f, 411.882f, 683.820f});
        cameraComp->SetPositionAndTarget({182.2f, -43.5f, 163.3f}, {0.0f, 0.0f, 0.0f});
        cameraComp->SetPerspective(45.0f);

        //TODO: Play heli animation
    }
    else {
        //TODO: Play gate break animation
        camera->SetTranslate({70.260f, 3.801f, 754.620f});
        cameraComp->SetPositionAndTarget({66.70f, 46.90f, -62.0f}, {0.0f, 0.0f, 1.0f});
        cameraComp->SetPerspective(45.0f);

        //Create object path for enemyt
        auto path = Scene::Object::Create<Object>();
        path->SetTranslate({70.222f, 4.138f, 900.0f});
        path->SetName("Gate");

        //Create the enemy that broke in
        auto enemy = Scene::Object::Create<Enemy>();
        enemy->SetTranslate({70.260f, 3.801f, 740.620f});
        enemy->SetScaleAxisXYZ({0.7f, 0.7f, 0.7f});
        enemy->SetRotationAxisXYZ({0.0f, 180.0f, 0.0f});
        enemy->SetMoveSpeed(0.1f);    // now reliably applied on the enemy's first Update() tick

        //TODO: Play gate animation
        gate->DestroyGate();
    }

    //BLACK SCREEN TRANSITION
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName("BlackScreen");
    blackScreen_ = blackScreenObj->AddComponent<ComponentBlackScreen>();
    blackScreen_->StartFadeIn();    // start fully black, fade INTO the game over screen

    //BUTTONS
    auto retryButtonObj = Scene::Object::Create<Object>();
    retryButtonObj->SetName("RetryButton");
    retryButton_ = retryButtonObj->AddComponent<ComponentButton>();
    retryButton_->Setup({66.0f, 400.0f}, {200.0f, 60.0f}, "RETRY", GetColor(0, 200, 0), [this]() {
        blackScreen_->StartFadeOut([]() { Scene::Change(Scene::GetScene<SniperVsZombie_MainStage>()); });
    });

    auto menuButtonObj = Scene::Object::Create<Object>();
    menuButtonObj->SetName("MenuButton");
    menuButton_ = menuButtonObj->AddComponent<ComponentButton>();
    menuButton_->Setup({66.0f, 480.0f}, {200.0f, 60.0f}, "MENU", GetColor(200, 0, 0), [this]() {
        blackScreen_->StartFadeOut([]() { Scene::Change(Scene::GetScene<SniperVsZombie_MainMenu>()); });
    });

    return true;
}

void SniperVsZombie_GameOver::Update()
{
    __super::Update();

    //CUTSCENE animation - simple slide, not a real scripted sequence yet
    if(isWin_) {
        // .lock() turns the weak_ptr into a temporary shared_ptr - it comes back
        // empty (nullptr-like) if the Helicopter object no longer exists, so always
        // check it before using it, instead of calling .lock() twice and assuming success
        if(auto heli = helicopter_.lock()) {
            float3 pos  = heli->GetTranslate();
            pos.y      += 20.0f * GetDeltaTime();    // rise
            pos.z      -= 40.0f * GetDeltaTime();    // fly off into the distance
            heli->SetTranslate(pos);
        }
    }
}

void SniperVsZombie_GameOver::PostDraw()
{
    __super::PostDraw();

    //RESULT TEXT
    SetFontSize(64);
    if(isWin_) {
        DrawTextWithAlign("Rescue complete", GetColor(0, 255, 0), 0, WINDOW_H / 2 - 32, 1);
        SetFontSize(50);
        DrawTextWithAlign("救出作戦完了", GetColor(0, 255, 0), 0, WINDOW_H / 2 + 32, 1);
        SetFontSize(64);
    }
    else {
        DrawTextWithAlign("Rescue failed", GetColor(255, 0, 0), 0, WINDOW_H / 2 - 32, 1);
        SetFontSize(50);
        DrawTextWithAlign("救出作戦完了", GetColor(255, 0, 0), 0, WINDOW_H / 2 + 32, 1);
        SetFontSize(64);
    }

    SetFontSize(30);

    DrawFormatString(WINDOW_W / 2 - 100, WINDOW_H / 2 + 130, GetColor(255, 255, 255), "スコア: %d", finalScore_);

    SetFontSize(20);

    //BUTTONS
    retryButton_->DrawButton();
    menuButton_->DrawButton();

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades
    blackScreen_->DrawBlackScreen();
}

}    // namespace SniperVsZombie
