//---------------------------------------------------------------------------
//!	@file	Tutorial_X.h
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include "Float2.h"
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>

namespace SniperVsZombie {
// forward declaration - a weak_ptr member only needs this, not the full class definition.
// avoids including SniperVsZombie_Helicopter.h here, which would double-include it
// whenever GameOver.cpp includes both this header AND Helicopter.h directly.
class Helicopter;

class SniperVsZombie_GameOver : public Scene::Base
{
public:
    BP_CLASS_DECL(SniperVsZombie_GameOver, u8"Game Over");

    //! @brief called by MainStage right before Scene::Change, so this scene knows how it ended
    void SetResult(bool isWin, int finalScore)
    {
        isWin_      = isWin;
        finalScore_ = finalScore;
    }

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

private:
    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene

    ComponentButtonPtr retryButton_;    // back to MainStage
    ComponentButtonPtr menuButton_;     // back to MainMenu

    std::weak_ptr<Helicopter> helicopter_;

    bool isWin_      = true;    //!< true = all civilians saved, false = gate destroyed
    int  finalScore_ = 0;       //!< score carried over from MainStage
};

}    // namespace SniperVsZombie
