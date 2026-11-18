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

//same, but x is the CENTER of the text, for subtitles and story lines(xが文字の中央になる版。字幕やストーリー用)
void DrawLabelCenter(int center_x, int y, unsigned int color, const char* format, ...);

//width of a text in pixels with the HUD font(HUDフォントでの文字の幅・ピクセル)
int GetTextWidth(const char* text);

//height of one line of text, use it to stack lines(1行の高さ。行を並べるのに使う)
int GetLineHeight();

//number of CHARACTERS (not bytes) in a Shift-JIS string. Japanese = 2 bytes but 1 character(Shift-JIS文字列の文字数(バイト数ではない)。日本語は2バイトで1文字)
//why Shift-JIS: plain "..." strings are stored as code page 932 by MSVC, and DxLib reads them that way(理由: MSVCは"..."をコードページ932で保存し、DxLibもそのまま読む)
int CountCharacters(const char* text);

//free the font. call it when the scene exits(フォントを解放する。シーン終了時に呼ぶ)
void ReleaseFont();

}    // namespace SniperVsZombie::Hud
