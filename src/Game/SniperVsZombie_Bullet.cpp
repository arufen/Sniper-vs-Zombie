//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Bullet.cpp
//! @brief	SniperVsZombie_Bullet
//---------------------------------------------------------------------------

#include "SniperVsZombie_Bullet.h"
#include "SniperVsZombie_MainStage.h"
#include <System/Component/ComponentCollisionLine.h>

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool Bullet::Init()
{
    Super::Init();

    SetName("Bullet");

    AddComponent<ComponentModel>("data/Game/Models/Bullet/bullet.mv1");

    auto col = AddComponent<ComponentCollisionLine>();

    // local-space line: from "just behind" to current position
    // since bullet is rotated to face movement direction (Z-forward)
    float offsetSize = 200.0f;
    col->SetLine({0.0f, 0.0f, speed_ + offsetSize}, {0.0f, 0.0f, 0.0f});

    col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);

    SetScaleAxisXYZ({0.2f});
    return true;
}

void Bullet::Update()
{
    AddTranslate(direction_ * speed_);

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

// 当たり判定が行われたときに呼ばれる関数
void Bullet::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    // 自分を削除する
    Scene::Object::Release(SharedThis());

    auto mat = GetWorldMatrix();
    auto rot = matrix::rotateY(-0.5 * DX_PI);
    mat      = mul(rot, mat);

    ComponentEffect::Object::Create("data/Sample/Effects/hit_eff.efkefc", mat);

    // 最後にこれを入れてください。ここでめり込みの解消を行っています。
    Super::OnHit(hit_info);
}

}    // namespace SniperVsZombie
