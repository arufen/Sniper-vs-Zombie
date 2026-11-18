//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_MainMenu.h
//! @brief	SniperVsZombie_MainMenu
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include "Float2.h"
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>

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

    ComponentButtonPtr playButton_;
    ComponentButtonPtr exitButton_;

    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene
};

}    // namespace SniperVsZombie
