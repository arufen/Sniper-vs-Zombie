#include <Game/Component/ComponentStalkerJump.h>
#include <Game/Component/ComponentAIState.h>
#include <Game/Component/ComponentWaypointFollower.h>
#include <Game/Component/State/ComponentStateWalk.h>
#include <System/Component/ComponentModel.h>
#include <algorithm>

namespace {
constexpr const char* JUMP_ANIM = "jump";    //anim name from ZombieBody(ZombieBodyのアニメ名)
}

void ComponentStalkerJump::Init()
{
    __super::Init();

    SetName<Component>("StalkerJump");
}

float ComponentStalkerJump::GetJumpAnimTime() const
{
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model || model->GetPlayAnimationName() != JUMP_ANIM)
        return -1.0f;

    //no % needed: the anim plays once (no loop), so this stops growing at the end(%は不要: ループしないので終わりで増えなくなる)
    return model->GetAnimationTime();
}

void ComponentStalkerJump::Update()
{
    __super::Update();

    auto owner = GetOwner();

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

            //take off on the exact anim frame where the feet leave the ground(足が離れるアニメのフレームで離陸)
            if(GetJumpAnimTime() >= takeoff_time_)
                StartJump();
            break;
        }

    case State::Jumping:
        {
            //the arc reads the same clock as the anim, so they cannot drift apart(放物線はアニメと同じ時計を読むので、ずれない)
            const float anim_time = GetJumpAnimTime();
            t_                    = std::clamp((anim_time - takeoff_time_) / (land_time_ - takeoff_time_), 0.0f, 1.0f);

            owner->SetTranslate(ArcPosition(start_, jump_points_[jump_index_], t_));

            //anim_time < 0 = something else replaced the jump anim, land now so we never get stuck in the air(<0 = 別アニメに変わった。空中で止まらないよう今着地)
            if(t_ >= 1.0f || anim_time < 0.0f)
                Land();
            break;
        }

    case State::Recover:
        {
            //wait for the stand-up part of the anim to finish(アニメの起き上がり部分が終わるまで待つ)
            auto        model     = owner->GetComponent<ComponentModel>();
            const float anim_time = GetJumpAnimTime();
            if(anim_time < 0.0f || anim_time >= anim_length_ || !model || !model->IsPlaying())
                FinishRecover();
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

    //PlayAnimation, not NoSame: the 2nd jump must restart the anim and reset its timer to 0(NoSameではなくPlayAnimation: 2回目のジャンプでもアニメを最初から再生し、タイマーを0に戻す)
    //no loop: the anim stops at the end, so GetAnimationTime() stops growing too(ループなし: 終わりで止まるので、GetAnimationTimeも増えなくなる)
    if(auto model = owner->GetComponent<ComponentModel>())
        model->PlayAnimation(JUMP_ANIM, false);

    t_     = 0.0f;
    state_ = State::Crouch;
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
    t_     = 1.0f;
    state_ = State::Recover;
}

void ComponentStalkerJump::FinishRecover()
{
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

ComponentStalkerJumpPtr ComponentStalkerJump::SetAnimTiming(float takeoff, float land, float length)
{
    //keep the order takeoff < land <= length. land - takeoff is used to divide, so never 0(takeoff < land <= length の順を守る。land-takeoffで割るので0にしない)
    takeoff_time_ = std::max(0.0f, takeoff);
    land_time_    = std::max(takeoff_time_ + 0.05f, land);
    anim_length_  = std::max(land_time_, length);
    return std::dynamic_pointer_cast<ComponentStalkerJump>(shared_from_this());
}

ComponentStalkerJumpPtr ComponentStalkerJump::SetJumpHeight(float height)
{
    jump_height_ = height;
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

            static const char* state_names[] = {"Waiting", "Crouch", "Jumping", "Recover", "Done"};
            ImGui::Text(u8"状態: %s", state_names[static_cast<int>(state_)]);
            ImGui::Text(u8"ジャンプ: %d / %d", static_cast<int>(jump_index_), static_cast<int>(jump_points_.size()));
            ImGui::Text(u8"アニメ時間: %.2f  放物線: %.2f", GetJumpAnimTime(), t_);

            //drag these while watching the anim time above to line up the feet(上のアニメ時間を見ながら動かして足を合わせる)
            float timing[3] = {takeoff_time_, land_time_, anim_length_};
            if(ImGui::DragFloat3(u8"離陸/着地/終了(秒)", timing, 0.01f, 0.0f, 10.0f))
                SetAnimTiming(timing[0], timing[1], timing[2]);
            ImGui::DragFloat(u8"ジャンプの高さ", &jump_height_, 0.5f);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentStalkerJump)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentStalkerJump)
