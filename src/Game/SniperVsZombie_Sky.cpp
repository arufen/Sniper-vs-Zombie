//---------------------------------------------------------------------------
//!	@file	TutorialX_Player.cpp
//! @brief	TutorialX_Player
//---------------------------------------------------------------------------

#include "SniperVsZombie_Sky.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool Sky::Init()
{
    Super::Init();

    SetName("Sky");

    // lambda helper to spawn a Sky piece fast
    auto makeSky = [this](float3 position, float3 scale) {
        auto g = AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage00_sky.mv1");
        SetScaleAxisXYZ(scale);
        SetTranslate(position);
        return g;
    };

    float3 position{0.0f, -581.0f, 0.0f};
    float3 scale{2.0f, 2.0f, 2.0f};

    auto Sky = makeSky(position, scale);

    return true;
}
}    // namespace SniperVsZombie
