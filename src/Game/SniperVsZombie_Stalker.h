//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Stalker.h
//! @brief	SniperVsZombie_Stalker
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <vector>

namespace SniperVsZombie {
USING_PTR(Stalker);

//phase 2 zombie: walks, jumps ground -> roof -> tower, then attacks the player(フェーズ2のゾンビ。歩いて、地上→屋根→塔へ跳び、プレイヤーを攻撃する)
//thin object like NormalZombie: body + wiring only, behavior is in components(NormalZombieと同じ薄いObject。体と組み立てだけ、行動はコンポーネント)
class Stalker : public Object
{
public:
    BP_OBJECT_DECL(Stalker, "SniperVsZombie::Stalker");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;

    void Update() override;

    // 当たり判定が行われたときに呼ばれる関数
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //ground waypoints to the jump spot(ジャンプ地点までの地上ウェイポイント)
    void SetPath(const std::vector<float3>& path);

    //landing points, e.g. {roof, tower}(着地点。例: {屋根, 塔})
    void SetJumpPoints(const std::vector<float3>& points);

    void SetMoveSpeed(float new_speed);
};
}    // namespace SniperVsZombie
