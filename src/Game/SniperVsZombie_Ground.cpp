//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Ground.cpp
//! @brief	SniperVsZombie_Ground
//---------------------------------------------------------------------------

#include "SniperVsZombie_Ground.h"
#include "ObjectNames.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool Ground::Init()
{
    Super::Init();

    SetName(Names::GROUND);

    // lambda helper to spawn a ground piece fast
    auto makeGround = [this](float3 position, float3 scale) {
        auto g = AddComponent<ComponentModel>("data/Game/Models/Stage/Grass16k.mv1");
        SetScaleAxisXYZ(scale);
        SetTranslate(position);
        return g;
    };

    float3 position{0.0f, 0.0f, 0.0f};
    float3 scale{1.0f, 1.0f, 1.0f};

    auto ground = makeGround(position, scale);

    AddComponent<ComponentCollisionModel>()->AttachToModel();

    return true;
}
}    // namespace SniperVsZombie
