#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <vector>
#include <string>
#include <string_view>

USING_PTR(ComponentWaypointFollower);

class ComponentWaypointFollower : public Component
{
public:
    BP_COMPONENT_DECL(ComponentWaypointFollower, u8"ウェイポイント追従");

    void Init() override;

    void Update() override;

    void GUI() override;

    //waypoints to visit in order(順番に通るウェイポイント)
    ComponentWaypointFollowerPtr SetPath(const std::vector<float3>& path);

    //walk speed, applied to ComponentStateWalk on the next Update(歩く速さ。次のUpdateでComponentStateWalkに反映)
    ComponentWaypointFollowerPtr SetMoveSpeed(float speed);

    //how close counts as "arrived" at a waypoint(どこまで近づいたら到着とみなすか)
    ComponentWaypointFollowerPtr SetReachDistance(float distance);

    //who to chase after the last waypoint, by object name(最後のウェイポイントの後に追う相手のObject名)
    ComponentWaypointFollowerPtr SetFinalTargetName(std::string_view name);

    //other components (like attack) stop the walking with this(攻撃など他のコンポーネントが歩きを止めるのに使う)
    void SetPaused(bool paused) { paused_ = paused; }

    bool IsPaused() const { return paused_; }

    //true when all waypoints are done(全ウェイポイントを通り終えたらtrue)
    bool IsPathFinished() const { return path_.empty() || path_index_ >= path_.size(); }

private:
    std::vector<float3> path_;              //waypoints(ウェイポイント)
    size_t              path_index_ = 0;    //waypoint we are heading to now(今向かっているウェイポイント番号)

    float move_speed_     = 3.0f;     //walk speed(歩く速さ)
    float reach_distance_ = 15.0f;    //arrive distance(到着判定の距離)

    std::string final_target_name_ = "Gate";    //chase this after the path ends(経路の後に追う相手)

    bool paused_           = false;    //true = do nothing, someone else is in control(trueなら何もしない。他が操作中)
    bool has_synced_speed_ = false;    //false = push move_speed_ to ComponentStateWalk on next Update(falseなら次のUpdateで速度を反映)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //only the tuning numbers are saved, not the path(調整用の数値だけ保存し、経路は保存しない)
    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(move_speed_), CEREAL_NVP(reach_distance_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentWaypointFollower, 1);
