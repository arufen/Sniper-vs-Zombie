//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_NormalZombie.h
//! @brief	SniperVsZombie_NormalZombie
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <vector>

namespace SniperVsZombie {
USING_PTR(NormalZombie);

//thin object: the body + wiring only. walking, attacking and dying are components(薄いObject。体と部品の組み立てだけ。歩く・攻撃・死亡はコンポーネント)
class NormalZombie : public Object
{
public:
    BP_OBJECT_DECL(NormalZombie, "SniperVsZombie::NormalZombie");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;

    void Update() override;

    //! @brief 終了
    void Exit() override;

    // 当たり判定が行われたときに呼ばれる関数
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //same functions as the old Enemy, so MainStage and GameOver keep working(旧Enemyと同じ関数。MainStageとGameOverがそのまま使える)
    void SetPath(const std::vector<float3>& path);
    void SetMoveSpeed(float newSpeed);
};
}    // namespace SniperVsZombie
