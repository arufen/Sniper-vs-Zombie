//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Player.cpp
//! @brief	SniperVsZombie_Player
//---------------------------------------------------------------------------

#include "SniperVsZombie_Player.h"
#include "Component/ComponentPlayerStats.h"

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool Player::Init()
{
    Super::Init();

    //enemies find the player by this name(敵はこの名前でプレイヤーを探す)
    SetName("Player");

    //body for enemies to touch. no UseGravity, the sniper stands still on the tower(敵が触れる体。狙撃手は塔の上で動かないので重力なし)
    auto col = AddComponent<ComponentCollisionCapsule>()->SetRadius(3.0f)->SetHeight(13.0f);
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);

    //HP and damage live here, so this file stays clean(HPやダメージはここに集約するので、このファイルは綺麗なまま)
    AddComponent<ComponentPlayerStats>();

    return true;
}

}    // namespace SniperVsZombie
