//---------------------------------------------------------------------------
//!	@file	HudText.cpp
//! @brief	one shared outlined font for all HUD text(HUDの文字すべてに使う、縁取り付きの共通フォント)
//---------------------------------------------------------------------------

#include "HudText.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace SniperVsZombie::Hud {

namespace {
int font_handle = -1;    //[DxLib] created on first use(初回使用時に作成)

const unsigned int EDGE_COLOR = 0x000000;    //black outline(黒い縁取り)

constexpr int TEXT_BUFFER_SIZE = 1024;    //was 256, long Japanese lines got cut off(以前は256。長い日本語の行が切れていた)

//the font is made only when it's first needed, so no Init call is required(必要になった時に作るので、初期化の呼び出しは不要)
int GetFont()
{
    if(font_handle == -1) {
        //nullptr = the default font, it has Japanese characters. ANTIALIASING_EDGE = smooth letters + outline(nullptr=標準フォント。日本語あり。ANTIALIASING_EDGE=なめらか+縁取り)
        font_handle = CreateFontToHandle(nullptr, FONT_SIZE, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8, -1, EDGE_SIZE);
    }
    return font_handle;
}
}    // namespace

void DrawLabel(int x, int y, unsigned int color, const char* format, ...)
{
    char    text[TEXT_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);    //fill in the %d etc. like printf(printfのように%dなどを埋める)
    va_end(args);

    DrawStringToHandle(x, y, text, color, GetFont(), EDGE_COLOR);
}

void DrawLabelRight(int right_x, int y, unsigned int color, const char* format, ...)
{
    char    text[TEXT_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);

    DrawStringToHandle(right_x - GetTextWidth(text), y, text, color, GetFont(), EDGE_COLOR);
}

void DrawLabelCenter(int center_x, int y, unsigned int color, const char* format, ...)
{
    char    text[TEXT_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);

    DrawStringToHandle(center_x - GetTextWidth(text) / 2, y, text, color, GetFont(), EDGE_COLOR);
}

int GetTextWidth(const char* text)
{
    return GetDrawStringWidthToHandle(text, static_cast<int>(strlen(text)), GetFont());
}

int CountCharacters(const char* text)
{
    int count = 0;
    for(const unsigned char* p = reinterpret_cast<const unsigned char*>(text); *p != 0; ++p) {
        //Shift-JIS lead byte = the first byte of a 2-byte character. skip its second byte(Shift-JISの先頭バイト=2バイト文字の1バイト目。2バイト目を飛ばす)
        const bool is_lead = (*p >= 0x81 && *p <= 0x9F) || (*p >= 0xE0 && *p <= 0xFC);
        if(is_lead && p[1] != 0)
            ++p;
        ++count;
    }
    return count;
}

int GetLineHeight()
{
    return FONT_SIZE + EDGE_SIZE * 2 + 2;    //letters + outline top and bottom + a little gap(文字+上下の縁取り+少しの隙間)
}

void ReleaseFont()
{
    if(font_handle != -1) {
        DeleteFontToHandle(font_handle);
        font_handle = -1;
    }
}

}    // namespace SniperVsZombie::Hud
