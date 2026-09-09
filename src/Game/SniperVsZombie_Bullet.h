//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Bullet.h
//! @brief	SniperVsZombie_Bullet
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Bullet);
class Bullet : public Object
{
public:
    BP_OBJECT_DECL(Bullet, "SniperVsZombie::Bullet");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;

    void Update() override;

    void SetDirection(float3 dir);

    void ResetDirection();

    // 当たり判定が行われたときに呼ばれる関数
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

private:
    float3 direction_       = {0, 1, 0};
    float  speed_           = 50.0f;
    float3 rotation_        = {0, 1, 0};
    float  time_to_destroy_ = 3.0f * 60.0f;    // seconds
};
}    // namespace SniperVsZombie
