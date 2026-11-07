#pragma once
#include <Game/Component/ComponentButton.h>

void ComponentButton::Setup(const Float2& pos, const Float2& size, const std::string& label, int color, std::function<void()> onClick)
{
    pos_      = pos;
    size_     = size;
    label_    = label;
    color_    = color;
    on_click_ = onClick;
}

void ComponentButton::Init()
{
    __super::Init();
}

void ComponentButton::Update()
{
    __super::Update();

    Float2 mousePos = GetMouseFloat2();
    bool   hovered  = CheckPointBoxHit(mousePos, pos_, size_);

    // grow toward HOVER_SCALE while hovered, shrink back to 1.0 otherwise
    float targetScale  = hovered ? HOVER_SCALE : 1.0f;
    scale_            += (targetScale - scale_) * SCALE_SPEED * GetDeltaTime();

    int  mouseInput   = GetMouseInput();
    bool leftDown     = (mouseInput & MOUSE_INPUT_LEFT) != 0;
    bool leftClicked  = leftDown && !wasLeftMouseDown_;    // only fire once, on the frame the click starts
    wasLeftMouseDown_ = leftDown;

    if(leftClicked && hovered) {
        if(on_click_)
            on_click_();
    }
}

void ComponentButton::DrawButton()
{
    // scale outward from the button's center so it grows evenly, not just down-right
    Float2 center     = {pos_.x + size_.x * 0.5f, pos_.y + size_.y * 0.5f};
    Float2 scaledSize = {size_.x * scale_, size_.y * scale_};
    Float2 scaledPos  = {center.x - scaledSize.x * 0.5f, center.y - scaledSize.y * 0.5f};

    DrawBoxAA(scaledPos.x, scaledPos.y, scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y, color_, 1);

    DrawFormatString(static_cast<int>(center.x - 20.0f), static_cast<int>(center.y - 8.0f), GetColor(255, 255, 255), "%s", label_.c_str());
}

void ComponentButton::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Button")) {
            // 有効/無効
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            // GUI上でオーナーから自分(ComponentButton)を削除します
            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::Text(u8"label: %s", label_.c_str());
            ImGui::Text(u8"scale: %.2f", scale_);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentButton)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentButton)
