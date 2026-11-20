//---------------------------------------------------------------------------
//!	@file	SettingsSliders.h
//! @brief	the settings panel shared by the main menu and the pause menu: box + 3 sliders(メインメニューとポーズメニューで共有する設定パネル: 箱+スライダー3本)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <Game/Component/ComponentSlider.h>
#include <functional>
#include <vector>

namespace SniperVsZombie {

//panel layout, 1280x720 screen. the button row goes at PANEL_BUTTON_Y(パネルの配置。1280x720画面。ボタンの行はPANEL_BUTTON_Yに置く)
namespace Panel {
constexpr int   LEFT     = 340;
constexpr int   TOP      = 100;
constexpr int   RIGHT    = 940;
constexpr int   BOTTOM   = 620;
constexpr int   CENTER_X = (LEFT + RIGHT) / 2;
constexpr float BUTTON_Y = 530.0f;    //top of the button row(ボタンの行の上端)
}    // namespace Panel

//plain helper owned by a scene, like ZombieSpawner. it creates its own slider objects(ZombieSpawnerと同じくシーンが持つ普通のクラス。スライダーのObjectは自分で作る)
class SettingsSliders
{
public:
    enum class Kind
    {
        Sensitivity,
        Bgm,
        Sfx,
    };
    using OnChangedFunc = std::function<void(Kind kind, float value)>;

    //make the 3 slider objects. on_changed runs AFTER the value is stored in Settings, e.g. to apply it live(スライダーObjectを3つ作る。on_changedはSettingsに保存した後に呼ばれる。即反映などに使う)
    void Create(OnChangedFunc on_changed = nullptr);

    //shown = can be dragged. hidden sliders skip their Update(表示=ドラッグできる。隠れたスライダーはUpdateしない)
    void SetVisible(bool visible);

    //copy the current Settings values into the sliders, call when the panel opens(今のSettingsの値をスライダーに写す。パネルを開く時に呼ぶ)
    void SyncFromSettings();

    //draw the dim background, the panel box and its title(暗い背景、パネルの箱、タイトルを描く)
    static void DrawPanel(const char* title);

    //draw the 3 labelled sliders inside the panel(パネル内に名前付きのスライダー3本を描く)
    void DrawSliders();

    //the slider objects, e.g. so the pause menu does not freeze them(スライダーのObject。ポーズで止めないようにする用など)
    const std::vector<ObjectPtr>& GetObjects() const { return objects_; }

private:
    ComponentSliderPtr     sensitivity_;
    ComponentSliderPtr     bgm_;
    ComponentSliderPtr     sfx_;
    std::vector<ObjectPtr> objects_;
};

}    // namespace SniperVsZombie
