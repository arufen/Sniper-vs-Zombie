#pragma once
#include "ComponentBaseState.h"

void ComponentBaseState::Init()
{
    __super::Init();
}

void ComponentBaseState::Update()
{
    __super::Update();
}

void ComponentBaseState::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(GetName().data())) {
            // 有効/無効
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            // GUI上でオーナーから自分(SampleObjectController)を削除します
            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentBaseState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentBaseState)
