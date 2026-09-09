//---------------------------------------------------------------------------
//!	@file	Tutorial_X.cpp
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include "SniperVsZombie_GameOver.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_MainStage.h"
#include "Hit.h"
#include <System/Component/ComponentCamera.h>

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
    cameraComp->SetPositionAndTarget({66.70f, -11.70f, -57.40f}, {0.0f, 0.0f, 1.0f});
    cameraComp->SetPerspective(60.0f);
    camera->SetTranslate(CAMERA_SPAWN_POSITION);
    camera->SetName("Camera");

    //MAP
    Scene::Object::Create<City>();
    auto   woodTower     = Scene::Object::Create<WoodTower>();
    float3 watchTowerPos = {437.0f, 100.0f, 550.0f};
    woodTower->SetTranslate({watchTowerPos});
    woodTower->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});
    return true;
}

void SniperVsZombie_GameOver::Update()
{
    __super::Update();
}

void SniperVsZombie_GameOver::PostDraw()
{
    __super::PostDraw();
}

}    // namespace SniperVsZombie
