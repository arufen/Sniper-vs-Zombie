//---------------------------------------------------------------------------
//!	@file	ComponentFlightPath.h
//! @brief	moves the owner through points on a timer, e.g. the helicopter in the win cutscene(タイマーで所有Objectを点から点へ動かす。例: 勝利カットシーンのヘリ)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <vector>

USING_PTR(ComponentFlightPath);

//how to use: AddPoint(W1, 0) then AddPoint(W2, 2.5) = fly from W1 to W2 in 2.5s(使い方: AddPoint(W1,0)の後AddPoint(W2,2.5)=W1からW2へ2.5秒で飛ぶ)
//the same point twice = hover there. each leg eases in/out, so it slows down before stopping(同じ点を2回=そこでホバリング。各区間はイーズするので止まる前に減速する)
class ComponentFlightPath : public Component
{
public:
    BP_COMPONENT_DECL(ComponentFlightPath, u8"飛行パス");

    void Update() override;

    //time = seconds to get here from the previous point. the first point's time is ignored(time=前の点からここまでの秒数。最初の点のtimeは無視)
    ComponentFlightPathPtr AddPoint(float3 position, float time);

    //turn to face where we fly. yaw_offset fixes a model that faces sideways (degrees)(飛ぶ方向を向く。yaw_offsetは横向きのモデルの補正・度)
    ComponentFlightPathPtr SetFaceTravel(bool face, float yaw_offset = 0.0f);

    //small up/down wobble (units), like a real hovering helicopter(小さな上下の揺れ。本物のホバリングのように)
    ComponentFlightPathPtr SetBob(float height);

    //place the owner at the first point and start(最初の点に置いて開始)
    void Play();

    bool  IsFinished() const { return !playing_; }
    float GetTime() const { return time_; }

private:
    struct Point
    {
        float3 position;
        float  time;
    };
    std::vector<Point> points_;

    bool  playing_     = false;
    float time_        = 0.0f;
    bool  face_travel_ = true;
    float yaw_offset_  = 0.0f;
    float bob_height_  = 0.0f;
    float yaw_         = 0.0f;     //current facing, turns smoothly(今の向き。なめらかに回る)
    bool  yaw_ready_   = false;    //false = snap to the first direction(false=最初の方向にすぐ合わせる)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};

CEREAL_CLASS_VERSION(ComponentFlightPath, 1);
