//---------------------------------------------------------------------------
//!	@file	PauseMenu.h
//! @brief	in-game pause panel: settings sliders + RESUME / RESTART / MENU(ゲーム中のポーズパネル: 設定スライダー+再開/やり直し/メニュー)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <Game/Component/ComponentButton.h>
#include "SettingsSliders.h"
#include <functional>
#include <vector>

namespace SniperVsZombie {

//plain helper owned by MainStage. it only shows the panel; freezing the world is the stage's job(MainStageが持つ普通のクラス。パネルを見せるだけ。世界を止めるのはステージの仕事)
class PauseMenu
{
public:
    //keys that open / close the menu. NOT Esc: WinMain.cpp quits the whole game on Esc(メニューを開閉するキー。Escではない: WinMain.cppはEscでゲームを終了する)
    static constexpr int KEY_1 = KEY_INPUT_P;
    static constexpr int KEY_2 = KEY_INPUT_TAB;

    struct Callbacks
    {
        std::function<void()>                                    on_resume;
        std::function<void()>                                    on_restart;
        std::function<void()>                                    on_menu;
        std::function<void(SettingsSliders::Kind kind, float v)> on_setting_changed;    //apply a setting live(設定を即反映)
    };

    //make the button and slider objects (hidden). call once in the scene's Init(ボタンとスライダーのObjectを作る(非表示)。シーンのInitで1回呼ぶ)
    void Create(const Callbacks& callbacks);

    //show / hide. closing saves the settings to the file(表示/非表示。閉じる時に設定をファイルに保存)
    void SetOpen(bool open);
    bool IsOpen() const { return open_; }

    //draw the panel. call from PostDraw, near the end so it covers the HUD(パネルを描く。HUDを覆うようPostDrawの終わり近くで呼ぶ)
    void Draw();

    //true if this object belongs to the menu, so the stage does not freeze it(このObjectがメニューの物ならtrue。ステージが止めないように)
    bool Owns(const Object* obj) const;

private:
    SettingsSliders        sliders_;
    ComponentButtonPtr     resume_;
    ComponentButtonPtr     restart_;
    ComponentButtonPtr     menu_;
    std::vector<ObjectPtr> objects_;    //every object the menu made(メニューが作った全Object)
    bool                   open_ = false;
};

}    // namespace SniperVsZombie
