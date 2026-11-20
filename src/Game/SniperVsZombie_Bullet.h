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

    //F9 hitbox view: new bullets draw their hit line in game(F9の当たり判定表示: 新しい弾が当たり判定の線を描く)
    static void SetShowHitbox(bool show);

private:
    //hit effect at the bullet's current position(弾の今の位置にヒットエフェクトを出す)
    void SpawnHitEffect();

    //set the WORLD-space hit line (the collision has DisableTransform)(ワールド座標の当たり判定線を設定。コリジョンはDisableTransform)
    void SetPathLine(const float3& from, const float3& to);

    bool reached_wall_ = false;    //this frame's path ended on the wall, remove next frame(このフレームの道が壁で終わった。次のフレームで消す)

    static inline bool show_hitbox_ = false;    //F9 view, shared by all bullets(F9表示。全弾で共有)

    float3 start_         = {0, 0, 0};    //where the shot started(撃った地点)
    float  stop_distance_ = FLT_MAX;      //distance to the wall, FLT_MAX = no wall(壁までの距離。FLT_MAX=壁なし)

    float3 direction_       = {0, 1, 0};
    float  speed_           = 50.0f;
    float3 rotation_        = {0, 1, 0};
    float  time_to_destroy_ = 3.0f * 60.0f;    // seconds
};
}    // namespace SniperVsZombie
