#pragma once
#include <Game/Component/State/ComponentStateIdleWalk.h>

void ComponentStateIdleWalk::Init()
{
    __super::Init();

    SetName<Component>("State IdleWalk");
}

void ComponentStateIdleWalk::Update()
{
    __super::Update();

    // オーナー(自分がAddComponentされたObject)を取得します
    // 処理されるときは必ずOwnerは存在しますので基本的にnullptrチェックは必要ありません
    auto owner = GetOwner();

    // 移動方向
    float3 dir{0, 0, 0};
    if(IsKey(key_up_))
        dir += {0, 0, -1};

    if(IsKey(key_down_))
        dir += {0, 0, 1};

    if(IsKey(key_right_))
        dir += {-1, 0, 0};

    if(IsKey(key_left_))
        dir += {1, 0, 0};

    // 移動キーが押されているか?
    if((float)length(dir) > 0.0f) {
        // 斜めが押されていることを考慮し、
        // その方向の移動スピードを1とし、スピードを掛け合わせる
        dir = normalize(dir);

        // キャラのローカル方向で移動をさせる
        owner->AddTranslate(dir * move_speed_, true);

        // モデルを移動の方向に向けます
        if(auto mdl = owner->GetComponent<ComponentModel>()) {
            auto rot = quaternion::rotation_axis({0, 1, 0}, front_rot_ * DegToRad);    //< Y軸1度回転

            mdl->SetRotationToVectorWithLimit(mul(dir, rot), rot_speed_);
            mdl->PlayAnimationNoSame("walk", true);
        }
    }
    else {
        // モデルを移動の方向に向けます
        if(auto mdl = owner->GetComponent<ComponentModel>()) {
            mdl->PlayAnimationNoSame("idle", true);
        }
    }
}

ComponentStateIdleWalkPtr ComponentStateIdleWalk::SetMoveSpeed(const float speed)
{
    move_speed_ = speed;
    return std::dynamic_pointer_cast<ComponentStateIdleWalk>(shared_from_this());
}
ComponentStateIdleWalkPtr ComponentStateIdleWalk::SetRotateSpeed(const float speed)
{
    rot_speed_ = speed;
    return std::dynamic_pointer_cast<ComponentStateIdleWalk>(shared_from_this());
}

const float ComponentStateIdleWalk::GetMoveSpeed() const
{
    return move_speed_;
}

const float ComponentStateIdleWalk::GetRotateSpeed() const
{
    return rot_speed_;
}

ComponentStateIdleWalkPtr ComponentStateIdleWalk::SetKeys(int up, int down, int left, int right)
{
    // 移動キーの設定
    key_up_    = up;
    key_down_  = down;
    key_left_  = left;
    key_right_ = right;
    return std::dynamic_pointer_cast<ComponentStateIdleWalk>(shared_from_this());
}

void ComponentStateIdleWalk::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentStateIdleWalk)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentStateIdleWalk)
