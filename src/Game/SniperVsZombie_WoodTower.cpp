//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_WoodTower.cpp
//! @brief	SniperVsZombie_WoodTower
//---------------------------------------------------------------------------

#include "SniperVsZombie_WoodTower.h"
#include "ObjectNames.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool WoodTower::Init()
{
    Super::Init();

    SetName(Names::WOOD_TOWER);

    // lambda helper to spawn a WoodTower piece fast
    auto makeWoodTower = [this](float3 position, float3 scale) {
        auto g = AddComponent<ComponentModel>("data/Game/Models/Stage/WoodTower.mv1");
        SetScaleAxisXYZ(scale);
        SetTranslate(position);
        return g;
    };

    float3 position{0.0f, 0.0f, 0.0f};
    float3 scale{1.0f, 1.0f, 1.0f};

    auto WoodTower = makeWoodTower(position, scale);

    AddComponent<ComponentCollisionModel>()->AttachToModel();

    return true;
}
}    // namespace SniperVsZombie
