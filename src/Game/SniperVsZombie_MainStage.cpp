//---------------------------------------------------------------------------
//!	@file	Tutorial_X.cpp
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include "SniperVsZombie_MainStage.h"
#include "SniperVsZombie_Player.h"
#include "SniperVsZombie_Ground.h"
#include "SniperVsZombie_Camera.h"

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_MainStage::Init()
{
    Scene::Object::Create<Player>();
    Scene::Object::Create<Ground>();
    Scene::Object::Create<Camera>();

    return true;
}

void SniperVsZombie_MainStage::Update()
{
}
}    // namespace SniperVsZombie
