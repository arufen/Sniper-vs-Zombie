//---------------------------------------------------------------------------
//!	@file	Tutorial_X.h
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include "Float2.h"

namespace SniperVsZombie {

class SniperVsZombie_GameOver : public Scene::Base
{
public:
    BP_CLASS_DECL(SniperVsZombie_GameOver, u8"Main Menu");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

private:
};

}    // namespace SniperVsZombie
