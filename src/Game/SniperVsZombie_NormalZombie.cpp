//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_NormalZombie.cpp
//! @brief	SniperVsZombie_NormalZombie
//---------------------------------------------------------------------------

#include "SniperVsZombie_NormalZombie.h"
#include "SniperVsZombie_MainStage.h"
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

    SetName("NormalZombie");

    //the spawn position is set by whoever creates this zombie (MainStage / GameOver)(出現位置は生成した側が設定する)

    //BODY(体)
    auto model = AddComponent<ComponentModel>("data/Game/Models/Enemy/Enemy.mv1");
    model->SetAnimation({
        {"attack", "data/Game/Models/Enemy/Anims/Attack.mv1", 0, 1.0f}, // Attack
        {  "walk",   "data/Game/Models/Enemy/Anims/Walk.mv1", 0, 3.0f}, // Walk
        {  "dead",   "data/Game/Models/Enemy/Anims/Dead.mv1", 0, 1.0f}, // Dead
    });

    //to stand
    auto feetCol = AddComponent<ComponentCollisionCapsule>();
    feetCol->SetRadius(3.0f)->SetHeight(10.0f)->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY)->UseGravity();

    auto bodyCol = AddComponent<ComponentCollisionCapsule>();
    bodyCol->SetRadius(2.0f)->SetHeight(9.0f)->SetTranslate({5.0f, -35.0f, 0.0f})->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
    bodyCol->AttachToModel("mixamorig:Hips");

    auto headCol = AddComponent<ComponentCollisionCapsule>();
    headCol->SetRadius(3.0f)->SetHeight(5.0f)->SetTranslate({0.0f, -11.0f, 0.0f})->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
    headCol->AttachToModel("mixamorig:Head");

    //BEHAVIOR(行動) - ComponentAIState auto-adds a ComponentStateWalk for us(ComponentAIStateがComponentStateWalkを自動で追加する)
    auto aiState = AddComponent<ComponentAIState>();
    aiState->SetTargetName("Gate");

    AddComponent<ComponentWaypointFollower>()->SetFinalTargetName("Gate");
    AddComponent<ComponentGateAttack>();

    //DEATH(死亡)
    auto death = AddComponent<ComponentEnemyDeath>();
    death->SetHeadCollision(headCol);
    death->SetOnDie([this]() {
        //a dead zombie does not walk or attack. ComponentStateWalk runs its own Update, so it must be removed too(死んだらもう歩かない・攻撃しない。ComponentStateWalkは自分でUpdateするので外す)
        RemoveComponent<ComponentWaypointFollower>();
        RemoveComponent<ComponentGateAttack>();
        RemoveComponent<ComponentStateWalk>();
    });

    //let the stage know one more enemy is alive(ステージに敵が1体増えたことを伝える)
    if(auto scene = dynamic_cast<SniperVsZombie_MainStage*>(Scene::GetCurrentScene()))
        scene->IncreaseEnemyCount();

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
    Super::OnHit(hit_info);

    auto name = hit_info.hit_collision_->GetOwner()->GetNameDefault();
    if(name == "Bullet") {
        if(auto death = GetComponent<ComponentEnemyDeath>())
            death->OnBulletHit(hit_info);
    }
}

//! @brief 終了
void NormalZombie::Exit()
{
    Super::Exit();

    //let the stage know this enemy is gone(ステージに敵が居なくなったことを伝える)
    if(auto scene = dynamic_cast<SniperVsZombie_MainStage*>(Scene::GetCurrentScene()))
        scene->DecreaseEnemyCount();
}

void NormalZombie::SetPath(const std::vector<float3>& path)
{
    if(auto follower = GetComponent<ComponentWaypointFollower>())
        follower->SetPath(path);
}

void NormalZombie::SetMoveSpeed(float newSpeed)
{
    if(auto follower = GetComponent<ComponentWaypointFollower>())
        follower->SetMoveSpeed(newSpeed);
}

}    // namespace SniperVsZombie
