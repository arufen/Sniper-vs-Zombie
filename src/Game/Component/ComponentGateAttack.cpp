#include <Game/Component/ComponentGateAttack.h>
#include <Game/Component/ComponentAIState.h>
#include <Game/Component/ComponentWaypointFollower.h>
#include <Game/Component/State/ComponentStateWalk.h>
#include <Game/SniperVsZombie_Gate.h>

void ComponentGateAttack::Init()
{
    __super::Init();

    SetName<Component>("GateAttack");
}

void ComponentGateAttack::Update()
{
    __super::Update();

    auto owner = GetOwner();

    auto  target   = Scene::Object::Get<Object>(target_name_);
    float distance = target ? static_cast<float>(length(target->GetTranslate() - owner->GetTranslate())) : 1e9f;

    if(distance >= attack_range_) {
        //too far. if we were attacking, let the walking start again(遠い。攻撃中だったなら歩きを再開させる)
        if(is_attacking_) {
            is_attacking_ = false;

            if(auto follower = owner->GetComponent<ComponentWaypointFollower>())
                follower->SetPaused(false);
        }
        return;
    }

    //ATTACK MODE - close enough, stop walking and start hitting(射程内。歩くのをやめて攻撃開始)
    is_attacking_ = true;

    if(auto follower = owner->GetComponent<ComponentWaypointFollower>())
        follower->SetPaused(true);

    owner->RemoveComponent<ComponentStateWalk>();

    if(auto ai = owner->GetComponent<ComponentAIState>())
        ai->ClearTargetPosition();

    if(auto model = owner->GetComponent<ComponentModel>())
        model->PlayAnimationNoSame("attack", true);

    attack_time_ -= static_cast<float>(GetDeltaTime());
    if(attack_time_ < 0.0f) {
        attack_time_ = attack_interval_;

        //only a real Gate takes damage. other objects with the same name are ignored(本物のGateだけがダメージを受ける。同名の別Objectは無視)
        if(auto gate = std::dynamic_pointer_cast<SniperVsZombie::Gate>(target)) {
            gate->TakeDamage(damage_);
        }
    }
}

ComponentGateAttackPtr ComponentGateAttack::SetTargetName(std::string_view name)
{
    target_name_ = name;
    return std::dynamic_pointer_cast<ComponentGateAttack>(shared_from_this());
}

ComponentGateAttackPtr ComponentGateAttack::SetAttackRange(float range)
{
    attack_range_ = range;
    return std::dynamic_pointer_cast<ComponentGateAttack>(shared_from_this());
}

ComponentGateAttackPtr ComponentGateAttack::SetAttackInterval(float interval)
{
    attack_interval_ = interval;
    attack_time_     = interval * 0.25f;
    return std::dynamic_pointer_cast<ComponentGateAttack>(shared_from_this());
}

ComponentGateAttackPtr ComponentGateAttack::SetDamage(float damage)
{
    damage_ = damage;
    return std::dynamic_pointer_cast<ComponentGateAttack>(shared_from_this());
}

void ComponentGateAttack::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Gate Attack")) {
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::Text(u8"攻撃中: %s", is_attacking_ ? "true" : "false");

            ImGui::DragFloat(u8"攻撃距離", &attack_range_, 0.5f);
            ImGui::DragFloat(u8"攻撃間隔(秒)", &attack_interval_, 0.1f);
            ImGui::DragFloat(u8"ダメージ", &damage_, 0.5f);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentGateAttack)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentGateAttack)
