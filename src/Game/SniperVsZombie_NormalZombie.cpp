//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_NormalZombie.cpp
//! @brief	SniperVsZombie_NormalZombie
//---------------------------------------------------------------------------

#include "SniperVsZombie_NormalZombie.h"
#include "ObjectNames.h"
#include "ZombieBody.h"
#include "Component/ComponentAIState.h"
#include "Component/ComponentWaypointFollower.h"
#include "Component/ComponentGateAttack.h"
#include "Component/ComponentEnemyDeath.h"
#include "Component/State/ComponentStateWalk.h"

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool NormalZombie::Init()
{
    Super::Init();

    SetName(Names::NORMAL_ZOMBIE);

    //the spawn position is set by whoever creates this zombie (ZombieSpawner / GameOver)(出現位置は生成した側が設定する)

    //BODY(体) - model + anims + feet/body/head capsules(モデル+アニメ+足・胴体・頭カプセル)
    auto body = AddZombieBody(this);

    //BEHAVIOR(行動) - ComponentAIState auto-adds a ComponentStateWalk for us(ComponentAIStateがComponentStateWalkを自動で追加する)
    auto aiState = AddComponent<ComponentAIState>();
    aiState->SetTargetName(Names::GATE);

    AddComponent<ComponentWaypointFollower>()->SetFinalTargetName(Names::GATE);
    AddComponent<ComponentGateAttack>()->SetTargetName(Names::GATE);

    //DEATH(死亡). score / hit marker are wired by the spawner with SetOnKilled(スコアやヒットマーカーはスポナーがSetOnKilledで繋ぐ)
    auto death = AddComponent<ComponentEnemyDeath>();
    death->SetHeadCollision(body.head);
    death->SetFeetCollision(body.feet);
    death->SetOnDie([this]() {
        //a dead zombie does not walk or attack. ComponentStateWalk runs its own Update, so it must be removed too(死んだらもう歩かない・攻撃しない。ComponentStateWalkは自分でUpdateするので外す)
        RemoveComponent<ComponentWaypointFollower>();
        RemoveComponent<ComponentGateAttack>();
        RemoveComponent<ComponentStateWalk>();
    });

    return true;
}

void NormalZombie::Update()
{
    Super::Update();

    //remove the dead body when its time is over(死体の時間が終わったら消す)
    if(auto death = GetComponent<ComponentEnemyDeath>()) {
        if(death->IsCorpseExpired())
            Scene::Object::Release(SharedThis());
    }
}

// 当たり判定が行われたときに呼ばれる関数
void NormalZombie::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto name = hit_info.hit_collision_->GetOwner()->GetNameDefault();
    if(name == Names::BULLET) {
        if(auto death = GetComponent<ComponentEnemyDeath>())
            death->OnBulletHit(hit_info);
    }

    //LAST, it solves the push-out of overlapping bodies(最後に呼ぶ。重なった体の押し出しを解決する)
    Super::OnHit(hit_info);
}

void NormalZombie::SetPath(const std::vector<float3>& path)
{
    if(auto follower = GetComponent<ComponentWaypointFollower>())
        follower->SetPath(path);
}

void NormalZombie::SetMoveSpeed(float new_speed)
{
    if(auto follower = GetComponent<ComponentWaypointFollower>())
        follower->SetMoveSpeed(new_speed);
}

}    // namespace SniperVsZombie
