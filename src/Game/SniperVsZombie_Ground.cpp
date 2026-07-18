//---------------------------------------------------------------------------
//!	@file	TutorialX_Player.cpp
//! @brief	TutorialX_Player
//---------------------------------------------------------------------------

#include "SniperVsZombie_Ground.h"
#include "SniperVsZombie_MainStage.h"

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool Ground::Init()
{
    Super::Init();

    SetName("Ground");

    //auto ground = Scene::Object::Create<Object>("Ground");
    auto ground = AddComponent<ComponentModel>("data/Game/Models/Stage/Box.mv1");
    ground->SetRotationAxisXYZ(float3{-90.0f, 0.0f, 0.0f});
    //SetScaleAxisXYZ(float3{ 10.0f, 0.0f, 10.0f });

    AddComponent<ComponentCollisionModel>()->AttachToModel();

    return true;
}
}    // namespace SniperVsZombie
