//---------------------------------------------------------------------------
//!	@file	TutorialX_Enemy.cpp
//! @brief	TutorialX_Enemy
//---------------------------------------------------------------------------

#include "SniperVsZombie_Enemy.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool Enemy::Init()
{
    Super::Init();

    return true;
}

void Enemy::Update()
{
    Super::Update();
}

// 当たり判定が行われたときに呼ばれる関数
void Enemy::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    Super::OnHit(hit_info);
}

}    // namespace SniperVsZombie
