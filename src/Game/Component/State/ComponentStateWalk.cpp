#pragma once
#include <Game/Component/State/ComponentStateWalk.h>
#include <Game/Component/ComponentAIState.h>

void ComponentStateWalk::Init()
{
    __super::Init();

    SetName<Component>("State Walk (Chase)");
}

void ComponentStateWalk::Update()
{
    __super::Update();

    // オーナー(自分がAddComponentされたObject)を取得します
    // 処理されるときは必ずOwnerは存在しますので基本的にnullptrチェックは必要ありません
    auto owner = GetOwner();

    float3 target_pos{};
    bool   have_target = false;

    if(auto ai = owner->GetComponent<ComponentAIState>()) {
        if(ai->HasTargetPosition()) {
            // chasing a raw point (e.g. a waypoint) instead of a named object
            target_pos  = ai->GetTargetPosition();
            have_target = true;
        }
        else if(auto target = Scene::Object::Get<Object>(ai->GetTargetName())) {
            target_pos  = target->GetTranslate();
            have_target = true;
        }
    }
    else if(auto target = Scene::Object::Get<Object>(target_name_)) {
        target_pos  = target->GetTranslate();
        have_target = true;
    }

    if(have_target) {
        owner->SetRotationToPositionWithLimit(target_pos, 3.0f);
    }

    // 移動方向
    float3 dir{0, 0, -1};
    // キャラのローカル方向で移動をさせる
    owner->AddTranslate(dir * move_speed_, true);

    // モデルを移動の方向に向けます
    if(auto mdl = owner->GetComponent<ComponentModel>()) {
        /*auto rot = quaternion::rotation_axis({0,1,0}, front_rot_ * DegToRad);
		mdl->SetRotationToVectorWithLimit(mul(dir, rot), rot_speed_);*/
        mdl->PlayAnimationNoSame("walk", true);
    }
}

ComponentStateWalkPtr ComponentStateWalk::SetMoveSpeed(const float speed)
{
    move_speed_ = speed;
    return std::dynamic_pointer_cast<ComponentStateWalk>(shared_from_this());
}
ComponentStateWalkPtr ComponentStateWalk::SetRotateSpeed(const float speed)
{
    rot_speed_ = speed;
    return std::dynamic_pointer_cast<ComponentStateWalk>(shared_from_this());
}

const float ComponentStateWalk::GetMoveSpeed() const
{
    return move_speed_;
}

const float ComponentStateWalk::GetRotateSpeed() const
{
    return rot_speed_;
}

void ComponentStateWalk::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();

        if(ImGui::TreeNode(u8"State Walk")) {
            //System Main
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            //Member
            ImGui::DragFloat(u8"Move Speed", &move_speed_, 0.001f);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentStateWalk)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentStateWalk)
