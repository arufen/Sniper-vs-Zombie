//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Camera.cpp
//! @brief	SniperVsZombie_Camera
//---------------------------------------------------------------------------

#include "SniperVsZombie_Camera.h"
#include "ObjectNames.h"
#include "SniperVsZombie_MainStage.h"
#include "SniperVsZombie_Bullet.h"
#include "GameSettings.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_Ground.h"
#include "Game/Component/ComponentFPSCameraController.h"
#include <algorithm>

namespace SniperVsZombie {

namespace {

//distance from start to the nearest building or ground along dir. FLT_MAX = nothing in the way(startからdir方向で一番近い建物か地面までの距離。FLT_MAX=何もない)
//one line check per shot, against the models that already have collision info (ComponentCollisionModel::AttachToModel)(1発につき1回の線判定。当たり情報を持つモデルに対して)
float FindWallDistance(const float3& start, const float3& dir)
{
    const float3 end     = start + dir * Config::SHOT_WALL_CHECK_RANGE;
    float        nearest = FLT_MAX;

    auto check = [&](const ObjectPtr& obj) {
        if(!obj)
            return;
        auto model = obj->GetComponent<ComponentModel>();
        if(!model)
            return;
        //MV1CollCheck_Line returns the hit polygon nearest to the start(MV1CollCheck_Lineは始点に一番近いポリゴンを返す)
        MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(model->GetModel(), -1, VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z));
        if(hit.HitFlag) {
            const float3 hit_pos = {hit.HitPosition.x, hit.HitPosition.y, hit.HitPosition.z};
            nearest              = std::min(nearest, static_cast<float>(length(hit_pos - start).x));
        }
    };

    check(Scene::Object::Get<City>());
    check(Scene::Object::Get<Ground>());
    return nearest;
}

}    // namespace
//! @brief 初期化
//! @return 初期化終了
bool Camera::Init()
{
    Super::Init();

    SetName(Names::CAMERA);

    auto cameraController = AddComponent<ComponentFPSCameraController>();
    cameraController->SetSensitivityScale(Settings::GetMouseSensitivity());    //from the settings menu(設定メニューから)
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

    const float3 start = GetTranslate();

    auto bullet = Scene::Object::Create<Bullet>();
    bullet->SetTranslate(start);
    bullet->SetDirection(forward);
    bullet->SetStopDistance(FindWallDistance(start, normalize(forward)));    //buildings stop the bullet(建物が弾を止める)

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

float Camera::GetReloadRatio() const
{
    if(shootIntervalTime <= 0.0f)
        return 0.0f;
    return std::clamp(shootCurrentTime / shootIntervalTime, 0.0f, 1.0f);
}

void Camera::Exit()
{
    Super::Exit();

    DeleteSoundMem(shootSoundHandle_);
}
}    // namespace SniperVsZombie
