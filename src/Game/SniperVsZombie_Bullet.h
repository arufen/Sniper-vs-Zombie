//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Bullet.h
//! @brief	SniperVsZombie_Bullet
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <cfloat>

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

    //a wall is this far from the start: the bullet stops there. call AFTER SetTranslate(壁が開始点からこの距離にある: 弾はそこで止まる。SetTranslateの後に呼ぶ)
    void SetStopDistance(float distance);

    //false = this point is behind the wall, the bullet can not hit it(false=この地点は壁の向こう。弾は当たらない)
    bool CanReach(const float3& point) const;

private:
    //hit effect at the bullet's current position(弾の今の位置にヒットエフェクトを出す)
    void SpawnHitEffect();

    float3 start_         = {0, 0, 0};    //where the shot started(撃った地点)
    float  stop_distance_ = FLT_MAX;      //distance to the wall, FLT_MAX = no wall(壁までの距離。FLT_MAX=壁なし)

    float3 direction_       = {0, 1, 0};
    float  speed_           = 50.0f;
    float3 rotation_        = {0, 1, 0};
    float  time_to_destroy_ = 3.0f * 60.0f;    // seconds
};
}    // namespace SniperVsZombie
