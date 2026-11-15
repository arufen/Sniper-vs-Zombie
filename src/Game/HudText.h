//---------------------------------------------------------------------------
//!	@file	HudText.h
//! @brief	one shared outlined font for all HUD text(HUDの文字すべてに使う、縁取り付きの共通フォント)
//---------------------------------------------------------------------------
#pragma once

namespace SniperVsZombie::Hud {

constexpr int FONT_SIZE = 26;    //text size in pixels, the old HUD used 20(文字の大きさ。前のHUDは20)
constexpr int EDGE_SIZE = 2;     //outline thickness in pixels(縁取りの太さ)

//named DrawLabel, not DrawText: windows.h has a DrawText macro that would rename it(DrawTextではなくDrawLabel: windows.hのDrawTextマクロに名前を変えられてしまう)
//draw outlined text. works like printf: Hud::DrawLabel(10, 50, white, "score: %d", score)(縁取り付きで文字を描く。printfと同じ使い方)
void DrawLabel(int x, int y, unsigned int color, const char* format, ...);

//same, but x is the RIGHT edge of the text, for the top-right tutorial lines(xが文字の右端になる版。右上の操作説明用)
void DrawLabelRight(int right_x, int y, unsigned int color, const char* format, ...);

//height of one line of text, use it to stack lines(1行の高さ。行を並べるのに使う)
int GetLineHeight();

//free the font. call it when the scene exits(フォントを解放する。シーン終了時に呼ぶ)
void ReleaseFont();

}    // namespace SniperVsZombie::Hud
