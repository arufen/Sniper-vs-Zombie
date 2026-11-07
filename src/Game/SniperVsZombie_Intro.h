//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Intro.h
//! @brief	Story intro screen, shown before the Main Menu
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include <Game/Component/ComponentBlackScreen.h>

namespace SniperVsZombie {

class SniperVsZombie_Intro : public Scene::Base
{
public:
    BP_CLASS_DECL(SniperVsZombie_Intro, u8"Intro");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

private:
    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene
    int                     blackScreenHandle_;

    bool wasLeftMouseDown_ = false;    // used to detect the frame a click starts, not held-down spam
};

}    // namespace SniperVsZombie
