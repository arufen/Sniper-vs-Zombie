//---------------------------------------------------------------------------
//!	@file	TutorialX_Player.cpp
//! @brief	TutorialX_Player
//---------------------------------------------------------------------------

#include "SniperVsZombie_Helicopter.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化終了
bool Helicopter::Init()
{
    Super::Init();

    SetName("Helicopter");

    float3 position{0.0f, 0.0f, 0.0f};
    float3 scale{1.0f, 1.0f, 1.0f};
    auto   helicopter = AddComponent<ComponentModel>("data/Game/Models/Stage/Source/Helicopter.mv1");
    //Animation
    helicopter->SetAnimation({
        {"idle", "data/Game/Models/Stage/Animation/Helicopter_engine_on.mv1", 0, 1.0f}, // Attack
    });
    helicopter->PlayAnimationNoSame("idle", true);

    SetScaleAxisXYZ(scale);
    SetTranslate(position);

    //AddComponent<ComponentCollisionModel>()->AttachToModel();

    return true;
}
}    // namespace SniperVsZombie
