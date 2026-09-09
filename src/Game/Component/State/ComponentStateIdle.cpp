#pragma once
#include <Game/Component/State/ComponentStateIdle.h>

void ComponentStateIdle::Init()
{
    __super::Init();

    SetName<Component>("State Idle");

    ChangeAnimation("idle", true);
}

void ComponentStateIdle::Update()
{
    __super::Update();

    // オーナー(自分がAddComponentされたObject)を取得します
    // 処理されるときは必ずOwnerは存在しますので基本的にnullptrチェックは必要ありません
    auto owner = GetOwner();
}

void ComponentStateIdle::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentStateIdle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentStateIdle)
