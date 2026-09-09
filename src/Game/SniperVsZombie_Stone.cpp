//---------------------------------------------------------------------------
//!	@file	TutorialX_Player.cpp
//! @brief	TutorialX_Player
//---------------------------------------------------------------------------

#include "SniperVsZombie_Stone.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool Stone::Init()
{
    Super::Init();

    SetName("Stone");

    // lambda helper to spawn a Stone piece fast
    auto makeStone = [this](float3 position, float3 scale) {
        auto g = AddComponent<ComponentModel>("data/Game/Models/Stage/Stone.mv1");
        SetScaleAxisXYZ(scale);
        SetTranslate(position);
        return g;
    };

    float3 position{0.0f, 0.0f, 0.0f};
    float3 scale{1.0f, 1.0f, 1.0f};

    auto Stone = makeStone(position, scale);

    AddComponent<ComponentCollisionModel>()->AttachToModel();

    return true;
}
}    // namespace SniperVsZombie
