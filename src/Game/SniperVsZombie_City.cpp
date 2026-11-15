//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_City.cpp
//! @brief	SniperVsZombie_City
//---------------------------------------------------------------------------

#include "SniperVsZombie_City.h"
#include "ObjectNames.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool City::Init()
{
    Super::Init();

    SetName(Names::CITY);

    // lambda helper to spawn a City piece fast
    auto makeCity = [this](float3 position, float3 scale) {
        auto g = AddComponent<ComponentModel>("data/Game/Models/Stage/city_normal_fix.mv1");
        SetScaleAxisXYZ(scale);
        SetTranslate(position);
        return g;
    };

    float3 position{0.0f, 0.0f, 0.0f};
    float3 scale{1.0f, 1.0f, 1.0f};

    auto City = makeCity(position, scale);

    auto col = AddComponent<ComponentCollisionModel>();
    col->AttachToModel();
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::GROUND);

    return true;
}
}    // namespace SniperVsZombie
