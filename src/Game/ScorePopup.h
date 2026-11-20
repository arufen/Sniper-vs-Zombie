//---------------------------------------------------------------------------
//!	@file	ScorePopup.h
//! @brief	"+1" / "HEADSHOT +2" text that floats up from a kill and fades(キル位置から浮き上がって消える「+1」「ヘッドショット +2」の文字)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <vector>

namespace SniperVsZombie {

//plain helper owned by the stage, like ZombieSpawner. not an Object(ZombieSpawnerと同じくステージが持つ普通のクラス。Objectではない)
class ScorePopups
{
public:
    //show a popup at a world position (the kill spot)(ワールド位置(キル地点)にポップアップを出す)
    void Add(const float3& world_pos, int score, bool headshot);

    //age the popups and remove old ones. call once per frame(ポップアップを古くし、期限切れを消す。毎フレーム1回)
    void Update(float delta_time);

    //draw them. call from PostDraw (needs the 3D camera for the world -> screen step)(描画。PostDrawから呼ぶ。ワールド→画面の変換に3Dカメラが必要)
    void Draw() const;

    void Clear() { popups_.clear(); }

private:
    struct Popup
    {
        float3 world_pos;
        int    score    = 0;
        bool   headshot = false;
        float  age      = 0.0f;    //seconds since it appeared(出てからの秒数)
    };
    std::vector<Popup> popups_;
};

}    // namespace SniperVsZombie
