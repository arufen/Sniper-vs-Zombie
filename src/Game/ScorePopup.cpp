//---------------------------------------------------------------------------
//!	@file	ScorePopup.cpp
//! @brief	"+1" / "HEADSHOT +2" text that floats up from a kill and fades(キル位置から浮き上がって消える「+1」「ヘッドショット +2」の文字)
//---------------------------------------------------------------------------
#include "ScorePopup.h"
#include "HudText.h"
#include "StageConfig.h"
#include <algorithm>

namespace SniperVsZombie {

namespace {
constexpr float HEAD_LIFT = 40.0f;    //start above the feet, about head height, world units(足元より上、だいたい頭の高さから始める・ワールド単位)
}

void ScorePopups::Add(const float3& world_pos, int score, bool headshot)
{
    popups_.push_back({
        world_pos + float3{0.0f, HEAD_LIFT, 0.0f},
          score, headshot, 0.0f
    });
}

void ScorePopups::Update(float delta_time)
{
    for(auto& p : popups_) p.age += delta_time;

    std::erase_if(popups_, [](const Popup& p) { return p.age >= Config::POPUP_TIME; });
}

void ScorePopups::Draw() const
{
    for(const auto& p : popups_) {
        //z between 0 and 1 = in front of the camera. behind us = skip, the player can not see the kill anyway(zが0〜1=カメラの前。後ろならスキップ。どうせキルは見えない)
        const VECTOR screen = ConvWorldPosToScreenPos(VGet(p.world_pos.x, p.world_pos.y, p.world_pos.z));
        if(screen.z <= 0.0f || screen.z >= 1.0f)
            continue;

        const float t     = std::clamp(p.age / Config::POPUP_TIME, 0.0f, 1.0f);    //0 = new, 1 = gone(0=出たて、1=消える)
        const int   x     = static_cast<int>(screen.x);
        const int   y     = static_cast<int>(screen.y - Config::POPUP_RISE * t);    //floats up(浮き上がる)
        const int   alpha = static_cast<int>(255.0f * (1.0f - t * t));              //stays clear, then fades fast at the end(はっきり見えて、最後に速く消える)

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
        if(p.headshot)
            Hud::DrawLabelCenter(x, y, GetColor(255, 90, 70), "ヘッドショット +%d", p.score);    //EN: HEADSHOT +n
        else
            Hud::DrawLabelCenter(x, y, GetColor(255, 255, 255), "+%d", p.score);
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

}    // namespace SniperVsZombie
