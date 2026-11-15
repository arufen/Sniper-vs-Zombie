#include <Game/Component/ComponentStalkerJump.h>
#include <Game/Component/ComponentAIState.h>
#include <Game/Component/ComponentWaypointFollower.h>
#include <Game/Component/State/ComponentStateWalk.h>
#include <algorithm>

void ComponentStalkerJump::Init()
{
    __super::Init();

    SetName<Component>("StalkerJump");
}

void ComponentStalkerJump::Update()
{
    __super::Update();

    auto        owner = GetOwner();
    const float dt    = static_cast<float>(GetDeltaTime());

    switch(state_) {
    case State::Waiting:
        {
            if(jump_index_ >= jump_points_.size())
                return;

            //no follower means no ground path, so jump right away(フォロワーが無い=地上経路なし。すぐ跳ぶ)
            auto follower = owner->GetComponent<ComponentWaypointFollower>();
            if(!follower || follower->IsPathFinished())
                StartCrouch();
            break;
        }

    case State::Crouch:
        {
            //look at where we will land, Y is ignored so it does not tilt(着地点の方を向く。Yは無視されるので傾かない)
            owner->SetRotationToPositionWithLimit(jump_points_[jump_index_], 5.0f);

            crouch_left_ -= dt;
            if(crouch_left_ <= 0.0f)
                StartJump();
            break;
        }

    case State::Jumping:
        {
            t_ = std::min(1.0f, t_ + dt / jump_duration_);

            owner->SetTranslate(ArcPosition(start_, jump_points_[jump_index_], t_));

            if(t_ >= 1.0f)
                Land();
            break;
        }

    case State::Done:
        break;
    }
}

float3 ComponentStalkerJump::ArcPosition(const float3& start, const float3& end, float t) const
{
    //straight line from start to end(開始から終了への直線)
    float3 base = start + (end - start) * t;

    //parabola: 0 at start and end, 1.0 at the middle(放物線: 開始と終了で0、真ん中で1.0)
    float lift = 4.0f * t * (1.0f - t) * jump_height_;

    return base + float3{0.0f, lift, 0.0f};
}

void ComponentStalkerJump::StartCrouch()
{
    auto owner = GetOwner();

    //take control from the walking(歩きから操作を奪う)
    if(auto follower = owner->GetComponent<ComponentWaypointFollower>())
        follower->SetPaused(true);

    owner->RemoveComponent<ComponentStateWalk>();

    if(auto ai = owner->GetComponent<ComponentAIState>())
        ai->ClearTargetPosition();

    //no crouch anim yet, the attack pose works as a "getting ready" pose(しゃがみアニメが無いので攻撃ポーズを構えとして使う)
    if(auto model = owner->GetComponent<ComponentModel>())
        model->PlayAnimationNoSame("jump", true);

    crouch_left_ = crouch_time_;
    state_       = State::Crouch;
}

void ComponentStalkerJump::StartJump()
{
    start_ = GetOwner()->GetTranslate();
    t_     = 0.0f;

    //gravity and a hand-moved Y fight each other, so gravity goes off in the air(重力と手動のY移動はケンカするので空中では重力OFF)
    SetFeetGravity(false);

    state_ = State::Jumping;
}

void ComponentStalkerJump::Land()
{
    GetOwner()->SetTranslate(jump_points_[jump_index_]);
    SetFeetGravity(true);

    jump_index_++;

    if(jump_index_ < jump_points_.size()) {
        //more jumps left, get ready for the next one(まだジャンプが残っている。次の準備)
        StartCrouch();
        return;
    }

    state_ = State::Done;

    if(on_finished_)
        on_finished_();
}

void ComponentStalkerJump::SetFeetGravity(bool on)
{
    if(auto feet = feet_collision_.lock()) {
        feet->UseGravity(on);
        //also turn the capsule off in the air, so roof edges do not push us off the arc(空中ではカプセルも切る。屋根の端に押し出されないように)
        feet->SetStatus(StatusBit::Enable, on);
    }
}

ComponentStalkerJumpPtr ComponentStalkerJump::SetJumpPoints(const std::vector<float3>& points)
{
    jump_points_ = points;
    jump_index_  = 0;
    return std::dynamic_pointer_cast<ComponentStalkerJump>(shared_from_this());
}

ComponentStalkerJumpPtr ComponentStalkerJump::SetFeetCollision(const std::shared_ptr<ComponentCollision>& feet)
{
    feet_collision_ = feet;
    return std::dynamic_pointer_cast<ComponentStalkerJump>(shared_from_this());
}

ComponentStalkerJumpPtr ComponentStalkerJump::SetJumpDuration(float seconds)
{
    jump_duration_ = std::max(0.05f, seconds);    //never 0, it is used to divide(割り算に使うので0にしない)
    return std::dynamic_pointer_cast<ComponentStalkerJump>(shared_from_this());
}

ComponentStalkerJumpPtr ComponentStalkerJump::SetJumpHeight(float height)
{
    jump_height_ = height;
    return std::dynamic_pointer_cast<ComponentStalkerJump>(shared_from_this());
}

ComponentStalkerJumpPtr ComponentStalkerJump::SetCrouchTime(float seconds)
{
    crouch_time_ = std::max(0.0f, seconds);
    return std::dynamic_pointer_cast<ComponentStalkerJump>(shared_from_this());
}

ComponentStalkerJumpPtr ComponentStalkerJump::SetOnFinished(std::function<void()> on_finished)
{
    on_finished_ = on_finished;
    return std::dynamic_pointer_cast<ComponentStalkerJump>(shared_from_this());
}

void ComponentStalkerJump::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Stalker Jump")) {
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            static const char* state_names[] = {"Waiting", "Crouch", "Jumping", "Done"};
            ImGui::Text(u8"状態: %s", state_names[static_cast<int>(state_)]);
            ImGui::Text(u8"ジャンプ: %d / %d", static_cast<int>(jump_index_), static_cast<int>(jump_points_.size()));

            if(ImGui::DragFloat(u8"滞空時間(秒)", &jump_duration_, 0.01f, 0.05f, 5.0f))
                jump_duration_ = std::max(0.05f, jump_duration_);
            ImGui::DragFloat(u8"ジャンプの高さ", &jump_height_, 0.5f);
            ImGui::DragFloat(u8"跳ぶ前の待ち時間(秒)", &crouch_time_, 0.05f, 0.0f, 10.0f);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentStalkerJump)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentStalkerJump)
