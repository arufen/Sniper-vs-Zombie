//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Stalker.cpp
//! @brief	SniperVsZombie_Stalker
//---------------------------------------------------------------------------

#include "SniperVsZombie_Stalker.h"
#include "ObjectNames.h"
#include "ZombieBody.h"
#include "Component/ComponentAIState.h"
#include "Component/ComponentWaypointFollower.h"
#include "Component/ComponentStalkerJump.h"
#include "Component/ComponentPlayerAttack.h"
#include "Component/ComponentEnemyDeath.h"
#include "Component/State/ComponentStateWalk.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool Stalker::Init()
{
    Super::Init();

    SetName(Names::STALKER);

    //BODY(体) - same body as NormalZombie. the jump turns gravity off on the feet while in the air(NormalZombieと同じ体。空中ではジャンプが足の重力を切る)
    auto body = AddZombieBody(this);

    //BEHAVIOR(行動) - ComponentAIState auto-adds a ComponentStateWalk for us(ComponentAIStateがComponentStateWalkを自動で追加する)
    auto aiState = AddComponent<ComponentAIState>();
    aiState->SetTargetName(Names::PLAYER);

    AddComponent<ComponentWaypointFollower>()->SetFinalTargetName(Names::PLAYER);

    //off until the last jump lands, so it never attacks from the ground or mid-air(最後の着地まではOFF。地上や空中からは攻撃しない)
    auto attack = AddComponent<ComponentPlayerAttack>();
    attack->SetTargetName(Names::PLAYER);
    attack->SetStatus(Component::StatusBit::Enable, false);

    auto jump = AddComponent<ComponentStalkerJump>();
    jump->SetFeetCollision(body.feet);
    jump->SetOnFinished([this]() {
        //on the tower now: walk the last bit to the player and start attacking(塔に着いた。残りを歩いてプレイヤーを攻撃開始)
        if(auto follower = GetComponent<ComponentWaypointFollower>())
            follower->SetPaused(false);
        if(auto atk = GetComponent<ComponentPlayerAttack>())
            atk->SetStatus(Component::StatusBit::Enable, true);
    });

    //DEATH(死亡). score / hit marker are wired by the spawner with SetOnKilled(スコアやヒットマーカーはスポナーがSetOnKilledで繋ぐ)
    auto death = AddComponent<ComponentEnemyDeath>();
    death->SetHeadCollision(body.head);
    death->SetOnDie([this]() {
        //a dead stalker does not walk, jump or attack. ComponentStateWalk runs its own Update, so it must be removed too(死んだらもう歩かない・跳ばない・攻撃しない。ComponentStateWalkも外す)
        RemoveComponent<ComponentWaypointFollower>();
        RemoveComponent<ComponentStalkerJump>();
        RemoveComponent<ComponentPlayerAttack>();
        RemoveComponent<ComponentStateWalk>();
    });

    return true;
}

void Stalker::Update()
{
    Super::Update();

    //remove the dead body when its time is over(死体の時間が終わったら消す)
    if(auto death = GetComponent<ComponentEnemyDeath>()) {
        if(death->IsCorpseExpired())
            Scene::Object::Release(SharedThis());
    }
}

// 当たり判定が行われたときに呼ばれる関数
void Stalker::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto name = hit_info.hit_collision_->GetOwner()->GetNameDefault();
    if(name == Names::BULLET) {
        if(auto death = GetComponent<ComponentEnemyDeath>())
            death->OnBulletHit(hit_info);
    }

    //LAST, it solves the push-out of overlapping bodies(最後に呼ぶ。重なった体の押し出しを解決する)
    Super::OnHit(hit_info);
}

void Stalker::SetPath(const std::vector<float3>& path)
{
    if(auto follower = GetComponent<ComponentWaypointFollower>())
        follower->SetPath(path);
}

void Stalker::SetJumpPoints(const std::vector<float3>& points)
{
    if(auto jump = GetComponent<ComponentStalkerJump>())
        jump->SetJumpPoints(points);
}

void Stalker::SetMoveSpeed(float new_speed)
{
    if(auto follower = GetComponent<ComponentWaypointFollower>())
        follower->SetMoveSpeed(new_speed);
}

}    // namespace SniperVsZombie
