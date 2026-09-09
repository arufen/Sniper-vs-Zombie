#pragma once
#include <Game/Component/ComponentAIState.h>
#include "State/ComponentStateIdleWalk.h"
#include "State/ComponentStateIdle.h"
#include "State/ComponentStateWalk.h"
#include <Game/Random.h>

void ComponentAIState::Init()
{
    __super::Init();
    //TODO AI初期化の作成
    SetName<Component>("AIState");

    auto walk = GetOwner()->AddComponent<ComponentStateWalk>();
    walk->SetMoveSpeed(GetRandomF(0.2f, 0.4f));
}

void ComponentAIState::Update()
{
    __super::Update();

    auto owner = GetOwner();

    //TODO AI動作の作成
}

void ComponentAIState::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        //...
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentAIState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentAIState)
