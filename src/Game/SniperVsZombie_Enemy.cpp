//---------------------------------------------------------------------------
//!	@file	TutorialX_Enemy.cpp
//! @brief	TutorialX_Enemy
//---------------------------------------------------------------------------

#include "SniperVsZombie_Enemy.h"
#include "SniperVsZombie_MainStage.h"
#include "Component/ComponentAIState.h"
#include "Component/State/ComponentStateWalk.h"
#include "SniperVsZombie_Gate.h"

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool Enemy::Init()
{
    Super::Init();

    SetName("Enemy");

    auto model = AddComponent<ComponentModel>("data/Game/Models/Enemy/Enemy.mv1");
    model->SetAnimation({
        {"attack", "data/Game/Models/Enemy/Anims/Attack.mv1", 0, 1.0f}, // Attack
        {  "walk",   "data/Game/Models/Enemy/Anims/Walk.mv1", 0, 3.0f}, // Walk
        {  "dead",   "data/Game/Models/Enemy/Anims/Dead.mv1", 0, 1.0f}, // Dead
    });

    SetTranslate({394.0f, 41.0f, 440.0f});

    //to stand
    auto feetCol = AddComponent<ComponentCollisionCapsule>();    //
    feetCol->SetRadius(3.0f)->SetHeight(10.0f)->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY)->UseGravity();

    auto bodyCol = AddComponent<ComponentCollisionCapsule>();    //
    bodyCol->SetRadius(2.0f)->SetHeight(9.0f)->SetTranslate({5.0f, -35.0f, 0.0f})->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
    bodyCol->AttachToModel("mixamorig:Hips");

    auto headCol = AddComponent<ComponentCollisionCapsule>();    //
    headCol->SetRadius(3.0f)->SetHeight(5.0f)->SetTranslate({0.0f, -11.0f, 0.0f})->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
    headCol->AttachToModel("mixamorig:Head");
    head_collision_ = headCol;    // remember this one specifically so OnHit() can tell it apart from body/feet

    auto aiState = AddComponent<ComponentAIState>();
    aiState->SetTargetName("Gate");

    // ComponentAIState::Init() auto-adds a ComponentStateWalk for us; just make sure
    // it uses our move speed instead of AIState's randomized default.
    if(auto walk = GetComponent<ComponentStateWalk>())
        walk->SetMoveSpeed(move_speed_);

    //Register to enemy count in MainStage
    /*	auto now_scene = Scene::GetCurrentScene();
		if(auto scene = dynamic_cast<SniperVsZombie_MainStage*>(now_scene))
		{
			scene->AddEnemyCount();
		}*/

    return true;
}

void Enemy::Update()
{
    Super::Update();

    if(isDead_) {
        time_to_destroy_ -= GetDeltaTime();
        if(time_to_destroy_ < 0.0f) {
            Scene::Object::Release(SharedThis());
        }
        return;    // dead enemies don't walk or attack
    }

    auto aiState    = GetComponent<ComponentAIState>();
    auto gateObject = Scene::Object::Get<Object>("Gate");

    float       distanceToGate  = gateObject ? static_cast<float>(length(gateObject->GetTranslate() - GetTranslate())) : 1e9f;
    const float minimumDistance = 50.0f;

    if(distanceToGate < minimumDistance) {
        // ATTACK MODE - close enough to the gate, stop walking and start hitting it
        RemoveComponent<ComponentStateWalk>();
        aiState->ClearTargetPosition();

        if(auto model = GetComponent<ComponentModel>())
            model->PlayAnimationNoSame("attack", true);

        attack_time_ -= GetDeltaTime();
        if(attack_time_ < 0.0f) {
            attack_time_ = attack_interval_;

            GatePtr gate = std::dynamic_pointer_cast<Gate>(gateObject);
            if(gate) {
                gate->TakeDamage(10);
            }
        }
    }
    else {
        // WALK MODE - not close enough yet, make sure we're still walking
        if(!GetComponent<ComponentStateWalk>()) {
            auto walk = AddComponent<ComponentStateWalk>();
            walk->SetMoveSpeed(move_speed_);
            if(auto model = GetComponent<ComponentModel>())
                model->PlayAnimationNoSame("walk", true);
        }

        bool onFinalWaypoint = path_.empty() || (path_index_ >= path_.size());

        if(!onFinalWaypoint) {
            // head for the current waypoint
            aiState->SetTargetPosition(path_[path_index_]);

            if(static_cast<float>(length(path_[path_index_] - GetTranslate())) < waypoint_reach_dist_) {
                path_index_++;
            }
        }
        else {
            // ran out of waypoints (or none were given) - beeline straight for the gate
            aiState->ClearTargetPosition();
            aiState->SetTargetName("Gate");
        }
    }
}

// 当たり判定が行われたときに呼ばれる関数
void Enemy::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    Super::OnHit(hit_info);

    auto name = hit_info.hit_collision_->GetOwner()->GetNameDefault();
    if(name == "Bullet") {
        // collision_ is OUR collider that got hit (feet/body/head) - hit_collision_ is
        // the OTHER side's collider (the bullet), which is why that one's used above
        // to check the name. Compare collision_ against the head capsule from Init().
        wasHeadshot_ = (hit_info.collision_ == head_collision_.lock());
        ToDeathState();

        auto now_scene = Scene::GetCurrentScene();
        if(auto scene = dynamic_cast<SniperVsZombie_MainStage*>(now_scene)) {
            scene->TriggerHitmark(wasHeadshot_);
        }
    }
}

void Enemy::ToDeathState()
{
    isDead_ = true;

    auto now_scene = Scene::GetCurrentScene();
    if(auto scene = dynamic_cast<SniperVsZombie_MainStage*>(now_scene)) {
        scene->AddScore(wasHeadshot_ ? 2 : 1);
    }

    if(auto model = GetComponent<ComponentModel>())
        model->PlayAnimationNoSame("dead", false);

    // ComponentStateWalk runs its own Update() independent of Enemy::Update(),
    // so it keeps moving/animating the corpse forward unless we remove it here.
    RemoveComponent<ComponentStateWalk>();

    // there are 3 capsules (feet, body, head) - RemoveComponent<T>() only strips
    // one at a time, so loop until every capsule is gone, not just one of them.
    while(auto col = GetComponent<ComponentCollisionCapsule>()) RemoveComponent(col);
}

}    // namespace SniperVsZombie
