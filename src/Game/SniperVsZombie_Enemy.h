//---------------------------------------------------------------------------
//!	@file	TutorialX_Enemy.h
//! @brief	TutorialX_Enemy
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include <vector>

namespace SniperVsZombie {
USING_PTR(Enemy);
class Enemy : public Object
{
public:
    BP_OBJECT_DECL(Enemy, "SniperVsZombie::Enemy");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;

    void Update() override;

    void ToDeathState();

    //! @brief set the waypoints this enemy walks through before it reaches the gate
    void SetPath(const std::vector<float3>& path)
    {
        path_       = path;
        path_index_ = 0;
    }

    /*void SetWalkSpeed(float speed);*/

    // 当たり判定が行われたときに呼ばれる関数
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

private:
    bool  isDead_          = false;
    float time_to_destroy_ = 3.0f;
    float attack_interval_ = 5.0f;
    float attack_time_     = attack_interval_ * 0.25f;    //offset

    // --- Headshot tracking ---
    std::weak_ptr<ComponentCollision>
         head_collision_;         //!< weak ref to the head capsule (weak so Enemy doesn't keep the component alive and block cleanup on exit)
    bool wasHeadshot_ = false;    //!< set in OnHit(), read by ToDeathState() to decide score

    // --- Pathfinding ---
    std::vector<float3> path_;                           //!< waypoints to walk through before attacking the gate
    size_t              path_index_          = 0;        //!< index of the waypoint we're currently heading to
    float               move_speed_          = 3.0f;     //!< walk speed
    float               waypoint_reach_dist_ = 15.0f;    //!< how close counts as "arrived" at a waypoint
};
}    // namespace SniperVsZombie
