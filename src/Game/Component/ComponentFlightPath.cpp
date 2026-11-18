//---------------------------------------------------------------------------
//!	@file	ComponentFlightPath.cpp
//! @brief	flight path(飛行パス)
//---------------------------------------------------------------------------
#include <Game/Component/ComponentFlightPath.h>
#include <algorithm>
#include <cmath>

namespace {
constexpr float TURN_SPEED = 3.0f;    //how fast the facing follows the travel direction(向きが進行方向に追いつく速さ)
constexpr float BOB_SPEED  = 2.0f;    //wobbles per second-ish(揺れの速さ)

float SmoothStep(float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

//shortest way from angle a to b in degrees, -180..180(角度aからbへの最短差・度)
float AngleDiff(float a, float b)
{
    float d = std::fmod(b - a + 540.0f, 360.0f) - 180.0f;
    return d;
}
}    // namespace

ComponentFlightPathPtr ComponentFlightPath::AddPoint(float3 position, float time)
{
    points_.push_back({position, std::max(0.0f, time)});
    return std::dynamic_pointer_cast<ComponentFlightPath>(shared_from_this());
}

ComponentFlightPathPtr ComponentFlightPath::SetFaceTravel(bool face, float yaw_offset)
{
    face_travel_ = face;
    yaw_offset_  = yaw_offset;
    return std::dynamic_pointer_cast<ComponentFlightPath>(shared_from_this());
}

ComponentFlightPathPtr ComponentFlightPath::SetBob(float height)
{
    bob_height_ = height;
    return std::dynamic_pointer_cast<ComponentFlightPath>(shared_from_this());
}

void ComponentFlightPath::Play()
{
    time_      = 0.0f;
    playing_   = points_.size() >= 2;
    yaw_ready_ = false;

    if(!points_.empty())
        GetOwner()->SetTranslate(points_.front().position);
}

void ComponentFlightPath::Update()
{
    __super::Update();

    if(!playing_)
        return;

    const float dt  = GetDeltaTime();
    time_          += dt;

    //find the leg we are on(今いる区間を探す)
    float  leg_start = 0.0f;
    size_t leg       = 1;
    while(leg < points_.size() && time_ > leg_start + points_[leg].time) {
        leg_start += points_[leg].time;
        leg++;
    }
    if(leg >= points_.size()) {
        leg      = points_.size() - 1;
        playing_ = false;    //arrived at the last point(最後の点に着いた)
    }

    const Point& from = points_[leg - 1];
    const Point& to   = points_[leg];
    const float  t    = playing_ ? SmoothStep(to.time > 0.0f ? (time_ - leg_start) / to.time : 1.0f) : 1.0f;

    float3 pos  = from.position + (to.position - from.position) * t;
    pos.y      += std::sin(time_ * BOB_SPEED * 3.14159f) * bob_height_;
    GetOwner()->SetTranslate(pos);

    //FACE TRAVEL: look at the next point that is really somewhere else (skip hover points)(進行方向を向く: 本当に別の場所にある次の点を見る。ホバー点は飛ばす)
    if(!face_travel_)
        return;

    for(size_t i = leg; i < points_.size(); ++i) {
        const float3 dir = points_[i].position - from.position;
        if(std::abs(dir.x) + std::abs(dir.z) < 0.01f)
            continue;

        //yaw for a model facing +Z: atan2(x, z)(+Zを向くモデルのヨー: atan2(x, z))
        const float target_yaw = std::atan2(static_cast<float>(dir.x), static_cast<float>(dir.z)) * (180.0f / 3.14159f) + yaw_offset_;
        if(!yaw_ready_) {
            yaw_       = target_yaw;
            yaw_ready_ = true;
        }
        else {
            yaw_ += AngleDiff(yaw_, target_yaw) * std::min(1.0f, TURN_SPEED * dt);
        }
        GetOwner()->SetRotationAxisXYZ({0.0f, yaw_, 0.0f});
        break;
    }
}

CEREAL_REGISTER_TYPE(ComponentFlightPath)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentFlightPath)
