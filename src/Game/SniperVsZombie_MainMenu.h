//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_MainMenu.h
//! @brief	SniperVsZombie_MainMenu
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include "Float2.h"
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>
#include <Game/Component/ComponentSlider.h>
#include "SettingsSliders.h"

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

    void Exit() override;

private:
    int titleHandle;
    int bgm_ = -1;    //[DxLib] menu music, -1 if the file is missing(メニューの音楽。ファイルが無ければ-1)

    //open/close the settings panel: the menu buttons and the panel widgets are never active at the same time(設定パネルを開閉。メニューのボタンとパネルの部品は同時に有効にならない)
    void SetSettingsOpen(bool open);

    void DrawSettingsPanel();

    ComponentButtonPtr playButton_;
    ComponentButtonPtr settingsButton_;
    ComponentButtonPtr exitButton_;

    //SETTINGS PANEL(設定パネル)
    bool               settingsOpen_ = false;
    SettingsSliders    settingsSliders_;    //sensitivity + BGM + SFX, shared with the pause menu(感度+BGM+効果音。ポーズメニューと共有)
    ComponentButtonPtr backButton_;

    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene
};

}    // namespace SniperVsZombie
