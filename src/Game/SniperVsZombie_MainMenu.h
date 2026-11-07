//---------------------------------------------------------------------------
//!	@file	Tutorial_X.h
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
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

private:
    int titleHandle;

    ComponentButtonPtr playButton_;
    ComponentButtonPtr exitButton_;

    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene
};

}    // namespace SniperVsZombie
