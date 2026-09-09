//---------------------------------------------------------------------------
//!	@file	TutorialX_Player.cpp
//! @brief	TutorialX_Player
//---------------------------------------------------------------------------

#include "SniperVsZombie_Gate.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {
void Gate::TakeDamage(float damage)
{
    if(HP_ > 0) {
        HP_ -= damage;
    }
}

void Gate::DestroyGate()
{
    //Remove current model
    if(auto model = GetComponent<ComponentModel>()) {
        RemoveComponent<ComponentModel>();
    }
    //Replace with the broken gate
    auto g = AddComponent<ComponentModel>("data/Game/Models/Stage/Gate_Crack.mv1");
    g->SetAnimation({
        {"broken", "data/Game/Models/Stage/Animation/Gate_Broken.mv1", 0, 1.0f}  // Attack
    });
    g->PlayAnimationNoSame("broken", false);
}

//! @brief 初期化
//! @return 初期化終了
bool Gate::Init()
{
    Super::Init();

    // lambda
    auto makeGate = [this](float3 position, float3 scale) {
        auto g = AddComponent<ComponentModel>("data/Game/Models/Stage/Gate.mv1");
        SetScaleAxisXYZ(scale);
        SetTranslate(position);
        return g;
    };

    SetName("Gate");

    float3 position{-390.0f, 9.0f, 440.0f};
    float3 scale{4.0, 4.0f, 4.0f};

    auto Gate = makeGate(position, scale);

    AddComponent<ComponentCollisionModel>()->AttachToModel();

    return true;
}
}    // namespace SniperVsZombie
