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

    //SFX
    shootSoundHandle_ = LoadSoundMem("data/Game/Audio/sfx_sniper_shot.mp3");

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

    // 音量の設定
    ChangeVolumeSoundMem(static_cast<int>(255.0f * 50.0f / 100.0f), shootSoundHandle_);

    //SFX - DX_PLAYTYPE_BACK plays it without blocking, and lets overlapping shots layer instead of cutting off
    PlaySoundMem(shootSoundHandle_, DX_PLAYTYPE_BACK);
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

void Camera::Exit()
{
    Super::Exit();

    DeleteSoundMem(shootSoundHandle_);
}
}    // namespace SniperVsZombie
