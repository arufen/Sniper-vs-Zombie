#pragma once
#include <Game/Component/ComponentPlayerState.h>
#include "ComponentStateIdleWalk.h"

void ComponentPlayerState::Init()
{
    __super::Init();

    GetOwner()
        ->AddComponent<ComponentStateIdleWalk>()    //
        ->SetMoveSpeed(0.3f)                        //
        ->SetRotateSpeed(20.0f);
}

void ComponentPlayerState::Update()
{
    __super::Update();

    auto owner = GetOwner();
    if(Input::IsKeyDown(KEY_INPUT_SPACE)) {
        /*if(!owner->GetComponent<ComponentStateThrow>())
		{
			owner->RemoveComponent<ComponentStateIdleWalk>();
			owner->AddComponent<ComponentStateThrow>();
		}*/
        //if(GetStateName() == "State IdleWalk")
        if(IsState<ComponentStateIdleWalk>()) {
            //ChangeState<ComponentStateThrow>();
        }
    }
}

void ComponentPlayerState::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("Player State")) {
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

CEREAL_REGISTER_TYPE(ComponentPlayerState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentPlayerState)
