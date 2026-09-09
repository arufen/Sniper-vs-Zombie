//---------------------------------------------------------------------------
//!	@file	Tutorial_X.h
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include "Float2.h"

namespace SniperVsZombie {

class SniperVsZombie_MainMenu : public Scene::Base
{
public:
    BP_CLASS_DECL(SniperVsZombie_MainMenu, u8"Main Menu");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

private:
    //! @brief updates hover-check + grow animation for one button. outHovered is set so Update() can react to clicks.
    void UpdateButton(Float2& pos, Float2& size, float& scale, bool& outHovered);

    //! @brief draws a button rectangle + label, scaled from its center
    void DrawButton(Float2& pos, Float2& size, float scale, const char* label, int color);

    int titleHandle;

    // --- Play button ---
    Float2 playButtonPos_   = {66.0f, 400.0f};    // top-left corner
    Float2 playButtonSize_  = {200.0f, 60.0f};
    float  playButtonScale_ = 1.0f;

    // --- Exit button ---
    Float2 exitButtonPos_   = {66.0f, 480.0f};
    Float2 exitButtonSize_  = {200.0f, 60.0f};
    float  exitButtonScale_ = 1.0f;

    bool wasLeftMouseDown_ = false;    // used to detect the frame a click starts, not held-down spam

    static constexpr float BUTTON_HOVER_SCALE = 1.15f;    // how big it grows on hover
    static constexpr float BUTTON_SCALE_SPEED = 8.0f;     // how fast it grows/shrinks (higher = snappier)
};

}    // namespace SniperVsZombie
