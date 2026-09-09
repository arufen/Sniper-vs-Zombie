//---------------------------------------------------------------------------
//!	@file	TutorialX_Camera.cpp
//! @brief	TutorialX_Camera
//---------------------------------------------------------------------------

#include "SniperVsZombie_Camera.h"
#include "SniperVsZombie_MainStage.h"
#include "SniperVsZombie_Bullet.h"
#include "Game/Component/ComponentFPSCameraController.h"

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool Camera::Init()
{
    Super::Init();

    SetName("Camera");

    auto cameraController = AddComponent<ComponentFPSCameraController>();
    SetTranslate({0.0f, 10.0f, 0.0f});
    //cameraController->SetEyePosition()

    //Gun Model
    auto gunModel = AddComponent<ComponentModel>("data/Game/Models/Player/Gun.mv1");
    gunModel->SetTranslate({3.57f, -1.17f, 9.33f});
    gunModel->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});
    gunModel->SetScaleAxisXYZ({0.03f, 0.03f, 0.03f});

    return true;
}

void Camera::Shoot()
{
    auto controller = GetComponent<ComponentFPSCameraController>();
    if(!controller)
        return;

    float3 forward = controller->GetForwardVector(10.0f);

    auto bullet = Scene::Object::Create<Bullet>();
    bullet->SetTranslate(GetTranslate());
    bullet->SetDirection(forward);
}

void Camera::Update()
{
    __super::Update();

    //Shoot mechanic
    {
        if(shootCurrentTime > 0.0f)
            shootCurrentTime -= GetDeltaTime();

        bool isAbleToShoot = shootCurrentTime <= 0.0f;

        if(IsMouseOn(MOUSE_INPUT_LEFT)) {
            if(isAbleToShoot) {
                Shoot();
                shootCurrentTime = shootIntervalTime;
            }
        }
    }
}
}    // namespace SniperVsZombie
