//---------------------------------------------------------------------------
//!	@file	Tutorial_X.h
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {

class SniperVsZombie_MainStage : public Scene::Base
{
public:
    static constexpr int MAX_ENEMIES = 5;

    BP_CLASS_DECL(SniperVsZombie_MainStage, u8"Sniper Main Stage");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

private:
};

}    // namespace SniperVsZombie
