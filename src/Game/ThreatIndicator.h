//---------------------------------------------------------------------------
//!	@file	ThreatIndicator.h
//! @brief	red warning for a dangerous enemy: marker on it, or an arrow at the screen edge(危険な敵の赤い警告: 敵の上のマーカー、または画面端の矢印)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>

namespace SniperVsZombie::Hud {

//call from PostDraw, once per enemy. world_pos = the enemy's feet, time = any timer that keeps going (for the blink)(PostDrawから敵1体につき1回呼ぶ。world_pos=敵の足元、time=進み続けるタイマー(点滅用))
//on screen  -> blinking red arrow above the enemy, pointing down at it(画面内 -> 敵の上で点滅する下向きの赤い矢印)
//off screen -> blinking red arrow at the screen edge, pointing the way to turn(画面外 -> 画面端で点滅する赤い矢印。振り向く方向を指す)
void DrawThreatIndicator(const float3& world_pos, float time);

}    // namespace SniperVsZombie::Hud
