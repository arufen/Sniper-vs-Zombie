//---------------------------------------------------------------------------
//!	@file	ThreatIndicator.cpp
//! @brief	red warning for a dangerous enemy: marker on it, or an arrow at the screen edge(危険な敵の赤い警告: 敵の上のマーカー、または画面端の矢印)
//---------------------------------------------------------------------------
#include "ThreatIndicator.h"
#include <algorithm>
#include <cfloat>
#include <cmath>

namespace SniperVsZombie::Hud {

namespace {

constexpr float EDGE_MARGIN = 70.0f;    //gap between the edge arrow and the screen edge, pixels(画面端の矢印と画面の端の間隔・ピクセル)
constexpr float EDGE_SIZE   = 34.0f;    //edge arrow length, pixels(画面端の矢印の長さ・ピクセル)
constexpr float MARKER_SIZE = 22.0f;    //on-screen arrow length, pixels(画面内の矢印の長さ・ピクセル)
constexpr float MARKER_LIFT = 30.0f;    //how high above the feet the marker sits, world units(マーカーを足元からどれだけ上に置くか・ワールド単位)
constexpr float OUTLINE     = 4.0f;     //black outline thickness, pixels(黒い縁の太さ・ピクセル)
constexpr float BLINK_SPEED = 8.0f;     //blink speed(点滅の速さ)
constexpr int   ALPHA_MIN   = 120;      //darkest moment of the blink(点滅の一番薄い時)

//filled triangle with its tip at (tip_x, tip_y), pointing along (dir_x, dir_y) = unit vector(先端が(tip_x, tip_y)で(dir_x, dir_y)方向を向く三角形。dirは長さ1)
void DrawArrow(float tip_x, float tip_y, float dir_x, float dir_y, float size, unsigned int color)
{
    //back center = tip moved backwards, wings = 90 degree turn of dir(後ろの中心=先端から後ろへ、翼=dirを90度回したもの)
    const float back_x = tip_x - dir_x * size;
    const float back_y = tip_y - dir_y * size;
    const float side_x = -dir_y * size * 0.6f;
    const float side_y = dir_x * size * 0.6f;
    DrawTriangleAA(tip_x, tip_y, back_x + side_x, back_y + side_y, back_x - side_x, back_y - side_y, color, TRUE);
}

//black arrow a bit bigger and a bit forward, then the red one on top = red arrow with a black outline(少し大きく前に出した黒い矢印の上に赤い矢印=黒縁付きの赤矢印)
void DrawOutlinedArrow(float tip_x, float tip_y, float dir_x, float dir_y, float size)
{
    DrawArrow(tip_x + dir_x * OUTLINE, tip_y + dir_y * OUTLINE, dir_x, dir_y, size + OUTLINE * 2.0f, GetColor(0, 0, 0));
    DrawArrow(tip_x, tip_y, dir_x, dir_y, size, GetColor(255, 40, 40));
}

}    // namespace

void DrawThreatIndicator(const float3& world_pos, float time)
{
    const float3 mark_pos = world_pos + float3{0.0f, MARKER_LIFT, 0.0f};
    const VECTOR mark     = VGet(mark_pos.x, mark_pos.y, mark_pos.z);

    const float screen_w = static_cast<float>(WINDOW_W);
    const float screen_h = static_cast<float>(WINDOW_H);
    const float center_x = screen_w * 0.5f;
    const float center_y = screen_h * 0.5f;

    //blink: alpha goes ALPHA_MIN..255 with a sin wave(点滅: sin波でアルファがALPHA_MIN〜255を往復)
    const float wave  = 0.5f + 0.5f * sinf(time * BLINK_SPEED);
    const int   alpha = ALPHA_MIN + static_cast<int>((255 - ALPHA_MIN) * wave);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

    //ON SCREEN: z between 0 and 1 means in front of the camera and inside the view depth(画面内: zが0〜1ならカメラの前で描画範囲内)
    const VECTOR screen    = ConvWorldPosToScreenPos(mark);
    const bool   on_screen = screen.z > 0.0f && screen.z < 1.0f && screen.x >= 0.0f && screen.x <= screen_w && screen.y >= 0.0f && screen.y <= screen_h;

    if(on_screen) {
        //arrow above the enemy, tip pointing down at it(敵の上の矢印。先端が下の敵を指す)
        DrawOutlinedArrow(screen.x, screen.y, 0.0f, 1.0f, MARKER_SIZE);
    }
    else {
        //OFF SCREEN: use camera space, NOT ConvWorldPosToScreenPos. behind the camera that one mirrors x/y and the arrow would lie(画面外: カメラ空間を使う。ConvWorldPosToScreenPosはカメラの後ろだとx/yが反転し、矢印が嘘をつく)
        //camera space: x = right, y = up, z = forward. stays correct even behind us(カメラ空間: x=右、y=上、z=前。後ろでも正しい)
        const VECTOR view  = VTransform(mark, GetCameraViewMatrix());
        float        dir_x = view.x;
        float        dir_y = -view.y;    //screen y goes DOWN, so flip(画面のyは下向きなので反転)

        float length = sqrtf(dir_x * dir_x + dir_y * dir_y);
        if(length < 0.0001f) {
            //exactly behind = no left/right info, just point down(真後ろ=左右の情報がない。下を指すだけ)
            dir_x  = 0.0f;
            dir_y  = 1.0f;
            length = 1.0f;
        }
        dir_x /= length;
        dir_y /= length;

        //ray from the screen center to a box EDGE_MARGIN inside the screen. the side it hits first wins(画面中央から、内側EDGE_MARGINの箱へ光線を飛ばす。先に当たる辺で止まる)
        const float half_w   = center_x - EDGE_MARGIN;
        const float half_h   = center_y - EDGE_MARGIN;
        const float hit_x    = (dir_x != 0.0f) ? half_w / fabsf(dir_x) : FLT_MAX;
        const float hit_y    = (dir_y != 0.0f) ? half_h / fabsf(dir_y) : FLT_MAX;
        const float distance = std::min(hit_x, hit_y);

        DrawOutlinedArrow(center_x + dir_x * distance, center_y + dir_y * distance, dir_x, dir_y, EDGE_SIZE);
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

}    // namespace SniperVsZombie::Hud
