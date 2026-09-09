//---------------------------------------------------------------------------
//!	@file	TutorialX_Player.cpp
//! @brief	TutorialX_Player
//---------------------------------------------------------------------------

#include "SniperVsZombie_City.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool City::Init()
{
    Super::Init();

    SetName("City");

    // lambda helper to spawn a City piece fast
    auto makeCity = [this](float3 position, float3 scale) {
        auto g = AddComponent<ComponentModel>("data/Game/Models/Stage/LowPolyCity.mv1");
        SetScaleAxisXYZ(scale);
        SetTranslate(position);
        return g;
    };

    float3 position{0.0f, 0.0f, 0.0f};
    float3 scale{1.0f, 1.0f, 1.0f};

    auto City = makeCity(position, scale);

    AddComponent<ComponentCollisionModel>()->AttachToModel();

    return true;
}
}    // namespace SniperVsZombie
