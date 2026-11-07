#include <Game/Component/ComponentWaypointFollower.h>
#include <Game/Component/ComponentAIState.h>
#include <Game/Component/State/ComponentStateWalk.h>

void ComponentWaypointFollower::Init()
{
    __super::Init();

    SetName<Component>("WaypointFollower");
}

void ComponentWaypointFollower::Update()
{
    __super::Update();

    //someone else (like the attack) is in control right now(攻撃など他のコンポーネントが操作中)
    if(paused_)
        return;

    auto owner = GetOwner();

    auto ai = owner->GetComponent<ComponentAIState>();
    if(!ai)
        return;

    //make sure we are walking(歩いている状態にする)
    auto walk = owner->GetComponent<ComponentStateWalk>();
    if(!walk) {
        //walk was removed before (by the attack), put it back(攻撃で外されていたら付け直す)
        walk = owner->AddComponent<ComponentStateWalk>();
        walk->SetMoveSpeed(move_speed_);

        if(auto model = owner->GetComponent<ComponentModel>())
            model->PlayAnimationNoSame("walk", true);

        has_synced_speed_ = true;
    }
    else if(!has_synced_speed_) {
        //done in Update, not Init, because ComponentStateWalk may not be ready in Init(InitだとComponentStateWalkが未準備の場合があるのでUpdateで行う)
        walk->SetMoveSpeed(move_speed_);
        has_synced_speed_ = true;
    }

    if(!IsPathFinished()) {
        //head for the current waypoint(今のウェイポイントへ向かう)
        ai->SetTargetPosition(path_[path_index_]);

        if(static_cast<float>(length(path_[path_index_] - owner->GetTranslate())) < reach_distance_) {
            path_index_++;
        }
    }
    else {
        //no waypoints left (or none given), go straight for the final target(ウェイポイントが無ければ最終目標へ直進)
        ai->ClearTargetPosition();
        ai->SetTargetName(final_target_name_);
    }
}

ComponentWaypointFollowerPtr ComponentWaypointFollower::SetPath(const std::vector<float3>& path)
{
    path_       = path;
    path_index_ = 0;
    return std::dynamic_pointer_cast<ComponentWaypointFollower>(shared_from_this());
}

ComponentWaypointFollowerPtr ComponentWaypointFollower::SetMoveSpeed(float speed)
{
    move_speed_       = speed;
    has_synced_speed_ = false;    //push the new speed on the next Update(次のUpdateで新しい速度を反映)
    return std::dynamic_pointer_cast<ComponentWaypointFollower>(shared_from_this());
}

ComponentWaypointFollowerPtr ComponentWaypointFollower::SetReachDistance(float distance)
{
    reach_distance_ = distance;
    return std::dynamic_pointer_cast<ComponentWaypointFollower>(shared_from_this());
}

ComponentWaypointFollowerPtr ComponentWaypointFollower::SetFinalTargetName(std::string_view name)
{
    final_target_name_ = name;
    return std::dynamic_pointer_cast<ComponentWaypointFollower>(shared_from_this());
}

void ComponentWaypointFollower::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Waypoint Follower")) {
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::Text(u8"ウェイポイント: %d / %d", static_cast<int>(path_index_), static_cast<int>(path_.size()));
            ImGui::Text(u8"停止中: %s", paused_ ? "true" : "false");

            if(ImGui::DragFloat(u8"移動速度", &move_speed_, 0.001f))
                has_synced_speed_ = false;

            ImGui::DragFloat(u8"到着距離", &reach_distance_, 0.5f);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentWaypointFollower)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentWaypointFollower)
