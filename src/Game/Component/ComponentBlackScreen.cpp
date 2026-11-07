#pragma once
#include <Game/Component/ComponentBlackScreen.h>

void ComponentBlackScreen::IncreaseAlpha()
{
    alpha_ += alpha_speed_;
    if(alpha_ > 255)
        alpha_ = 255;
}

void ComponentBlackScreen::DecreaseAlpha()
{
    alpha_ -= alpha_speed_;
    if(alpha_ < 0)
        alpha_ = 0;
}

void ComponentBlackScreen::StartFadeOut(std::function<void()> onComplete)
{
    fade_state_           = State::FadingOut;
    on_fade_out_complete_ = onComplete;
    isInTransition_       = true;
}

void ComponentBlackScreen::StartFadeIn()
{
    alpha_          = 255;    // start fully black, then fade down to clear
    fade_state_     = State::FadingIn;
    isInTransition_ = true;
}

void ComponentBlackScreen::Init()
{
    __super::Init();
}

void ComponentBlackScreen::Update()
{
    __super::Update();

    switch(fade_state_) {
    case State::FadingOut:
        IncreaseAlpha();
        if(isFullyOpaque()) {
            fade_state_     = State::Idle;
            isInTransition_ = false;

            // pull the callback out BEFORE calling it - Scene::Change() will likely
            // destroy this whole component, so touching our own members after is unsafe
            auto callback         = on_fade_out_complete_;
            on_fade_out_complete_ = nullptr;
            if(callback)
                callback();
        }
        break;

    case State::FadingIn:
        DecreaseAlpha();
        if(isFullyClear()) {
            fade_state_     = State::Idle;
            isInTransition_ = false;
        }
        break;

    case State::Idle:
    default:
        break;
    }
}

void ComponentBlackScreen::DrawBlackScreen()
{
    if(alpha_ <= 0)
        return;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);
    DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ComponentBlackScreen::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Black Screen")) {
            // 有効/無効
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            // GUI上でオーナーから自分(ComponentBlackScreen)を削除します
            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::Text(u8"alpha: %d", alpha_);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentBlackScreen)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentBlackScreen)
