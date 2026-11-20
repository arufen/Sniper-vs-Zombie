//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Bullet.cpp
//! @brief	SniperVsZombie_Bullet
//---------------------------------------------------------------------------

#include "SniperVsZombie_Bullet.h"
#include "ObjectNames.h"
#include "SniperVsZombie_MainStage.h"
#include "StageConfig.h"
#include <System/Component/ComponentCollisionLine.h>

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool Bullet::Init()
{
    Super::Init();

    SetName(Names::BULLET);

    auto model = AddComponent<ComponentModel>("data/Game/Models/Bullet/bullet.mv1");
    model->SetScaleAxisXYZ({0.05f});

    auto col = AddComponent<ComponentCollisionLine>();

    // local-space line: from "just behind" to current position
    // since bullet is rotated to face movement direction (Z-forward)
    float offsetSize = 200.0f;
    col->SetLine({0.0f, 0.0f, speed_ + offsetSize}, {0.0f, 0.0f, 0.0f});

    col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY /* | 
									(u32)ComponentCollision::CollisionGroup::GROUND*/
    );

    SetScaleAxisXYZ({0.2f});
    return true;
}

void Bullet::Update()
{
    AddTranslate(direction_ * speed_);

    //WALL: flew past the wall distance = it hit the building. snap onto the wall, show the hit, remove(壁: 壁の距離を越えた=建物に当たった。壁の位置に戻してエフェクトを出し、消す)
    if(length(GetTranslate() - start_).x >= stop_distance_) {
        SetTranslate(start_ + direction_ * stop_distance_);
        SpawnHitEffect();
        Scene::Object::Release(SharedThis());
        return;
    }

    //Destroy this object after 5 seconds
    time_to_destroy_--;

    if(time_to_destroy_ <= 0) {
        Scene::Object::Release(SharedThis());
    }
}

void Bullet::SetDirection(float3 dir)
{
    direction_ = normalize(dir);

    SetRotationToVector(direction_);
}

void Bullet::ResetDirection()
{
    SetRotationToVector(direction_);
}

void Bullet::SetStopDistance(float distance)
{
    start_         = GetTranslate();
    stop_distance_ = distance;
}

bool Bullet::CanReach(const float3& point) const
{
    return length(point - start_).x <= stop_distance_ + Config::SHOT_WALL_TOLERANCE;
}

void Bullet::SpawnHitEffect()
{
    auto mat = GetWorldMatrix();
    auto rot = matrix::rotateY(-0.5 * DX_PI);
    mat      = mul(rot, mat);

    ComponentEffect::Object::Create("data/Sample/Effects/hit_eff.efkefc", mat);
}

// 当たり判定が行われたときに呼ばれる関数
void Bullet::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    //the collision line reaches far ahead, so it can touch a zombie BEHIND a wall. ignore that one(当たり判定の線は前方に長いので、壁の向こうのゾンビに触れることがある。それは無視)
    if(const Object* other = hit_info.hit_collision_->GetOwner(); other && !CanReach(other->GetTranslate()))
        return;

    // 自分を削除する
    Scene::Object::Release(SharedThis());

    SpawnHitEffect();

    // 最後にこれを入れてください。ここでめり込みの解消を行っています。
    Super::OnHit(hit_info);
}

}    // namespace SniperVsZombie
