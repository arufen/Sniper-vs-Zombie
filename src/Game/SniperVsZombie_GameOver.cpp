//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_GameOver.cpp
//! @brief	SniperVsZombie_GameOver
//---------------------------------------------------------------------------
#include "SniperVsZombie_GameOver.h"
#include "ObjectNames.h"
#include "StageConfig.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_Sky.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_Helicopter.h"
#include "SniperVsZombie_NormalZombie.h"
#include "SniperVsZombie_MainStage.h"
#include "SniperVsZombie_MainMenu.h"
#include "Component/State/ComponentStateWalk.h"
#include "SniperVsZombie_Gate.h"
#include "Hit.h"
#include <System/Component/ComponentCamera.h>
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_GameOver::Init()
{
    //CAMERA
    auto camera     = Scene::Object::Create<Object>();
    auto cameraComp = camera->AddComponent<ComponentCamera>();
    camera->SetName(Names::CAMERA);

    //MAP - same positions as MainStage, from StageConfig.h(マップ。MainStageと同じ位置をStageConfig.hから)
    Scene::Object::Create<City>();
    auto woodTower = Scene::Object::Create<WoodTower>();
    woodTower->SetTranslate(Config::WOOD_TOWER_POSITION);
    woodTower->SetRotationAxisXYZ(Config::WOOD_TOWER_ROTATION);

    //Sky
    Scene::Object::Create<Sky>();

    //Load helicopter
    auto heli = Scene::Object::Create<Helicopter>();
    heli->SetTranslate(Config::HELI_POSITION);
    heli->SetScaleAxisXYZ(Config::HELI_SCALE);
    heli->SetRotationAxisXYZ(Config::HELI_ROTATION_CUTSCENE);
    helicopter_ = heli;    // remember for cutscene animation

    //GATE - a different name, so the cutscene zombie walks to the fake "Gate" point below instead(別名にする。カットシーンのゾンビは下の偽"Gate"地点へ歩く)
    auto gate = Scene::Object::Create<Gate>();
    gate->SetName(Names::GATE_CUTSCENE);
    gate->RemoveComponent<ComponentCollisionModel>();    // don't need collision for the cutscene
    gate->SetTranslate(Config::GATE_POSITION);
    gate->SetScaleAxisXYZ(Config::GATE_SCALE);
    gate->SetRotationAxisXYZ(Config::GATE_ROTATION);

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

        //fake "Gate" point behind the gate, the zombie walks through toward it(ゲートの奥の偽"Gate"地点。ゾンビはそこへ向かって通り抜ける)
        auto path = Scene::Object::Create<Object>();
        path->SetTranslate({70.222f, 4.138f, 900.0f});
        path->SetName(Names::GATE);

        //Create the enemy that broke in
        auto enemy = Scene::Object::Create<NormalZombie>();
        enemy->SetTranslate({70.260f, 3.801f, 740.620f});
        enemy->SetScaleAxisXYZ({0.7f, 0.7f, 0.7f});
        enemy->SetRotationAxisXYZ({0.0f, 180.0f, 0.0f});
        enemy->SetMoveSpeed(0.1f);    // now reliably applied on the enemy's first Update() tick

        //TODO: Play gate animation
        gate->DestroyGate();
    }

    //BLACK SCREEN TRANSITION
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName(Names::BLACK_SCREEN);
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
