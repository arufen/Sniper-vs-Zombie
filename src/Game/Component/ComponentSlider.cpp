//---------------------------------------------------------------------------
//!	@file	ComponentSlider.cpp
//! @brief	a horizontal slider you drag with the mouse(マウスでドラッグする横スライダー)
//---------------------------------------------------------------------------
#include <Game/Component/ComponentSlider.h>
#include <algorithm>
#include <cmath>

ComponentSliderPtr ComponentSlider::Setup(Float2 pos, float width, float min_value, float max_value, float value)
{
    pos_       = pos;
    width_     = std::max(1.0f, width);
    min_value_ = min_value;
    max_value_ = std::max(min_value, max_value);
    value_     = SnapAndClamp(value);
    return std::dynamic_pointer_cast<ComponentSlider>(shared_from_this());
}

ComponentSliderPtr ComponentSlider::SetStep(float step)
{
    step_  = std::max(0.0f, step);
    value_ = SnapAndClamp(value_);
    return std::dynamic_pointer_cast<ComponentSlider>(shared_from_this());
}

ComponentSliderPtr ComponentSlider::SetOnChanged(OnChangedFunc on_changed)
{
    on_changed_ = on_changed;
    return std::dynamic_pointer_cast<ComponentSlider>(shared_from_this());
}

ComponentSliderPtr ComponentSlider::SetValue(float value)
{
    value_ = SnapAndClamp(value);
    return std::dynamic_pointer_cast<ComponentSlider>(shared_from_this());
}

float ComponentSlider::SnapAndClamp(float value) const
{
    if(step_ > 0.0f)
        value = min_value_ + std::round((value - min_value_) / step_) * step_;    //snap to the nearest step(一番近い単位に合わせる)
    return std::clamp(value, min_value_, max_value_);
}

void ComponentSlider::Init()
{
    __super::Init();
}

void ComponentSlider::Update()
{
    __super::Update();

    Float2 mouse = GetMouseFloat2();

    //hit area = the bar, made as tall as the knob so it's easy to grab(当たり範囲=バー。つまみの高さまで広げて掴みやすく)
    const bool on_bar =
        mouse.x >= pos_.x - KNOB_RADIUS && mouse.x <= pos_.x + width_ + KNOB_RADIUS && mouse.y >= pos_.y - KNOB_RADIUS && mouse.y <= pos_.y + KNOB_RADIUS;
    hovered_ = on_bar;

    const bool left_down    = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
    const bool left_clicked = left_down && !was_left_down_;
    was_left_down_          = left_down;

    //start dragging when the click starts on the bar, keep dragging even if the mouse leaves it(バー上でクリック開始でドラッグ開始。マウスが外れてもドラッグを続ける)
    if(left_clicked && on_bar)
        dragging_ = true;
    if(!left_down)
        dragging_ = false;

    if(dragging_) {
        const float ratio     = std::clamp((mouse.x - pos_.x) / width_, 0.0f, 1.0f);
        const float new_value = SnapAndClamp(min_value_ + (max_value_ - min_value_) * ratio);
        if(new_value != value_) {
            value_ = new_value;
            if(on_changed_)
                on_changed_(value_);
        }
    }

    const float target_scale  = (hovered_ || dragging_) ? HOVER_SCALE : 1.0f;
    knob_scale_              += (target_scale - knob_scale_) * std::min(1.0f, SCALE_SPEED * GetDeltaTime());
}

void ComponentSlider::DrawSlider()
{
    const float ratio  = (max_value_ > min_value_) ? (value_ - min_value_) / (max_value_ - min_value_) : 0.0f;
    const float knob_x = pos_.x + width_ * ratio;
    const float top    = pos_.y - BAR_HEIGHT * 0.5f;
    const float bottom = pos_.y + BAR_HEIGHT * 0.5f;

    DrawBoxAA(pos_.x, top, pos_.x + width_, bottom, GetColor(70, 70, 70), TRUE);    //empty part(空の部分)
    DrawBoxAA(pos_.x, top, knob_x, bottom, GetColor(0, 200, 0), TRUE);              //filled part, same green as PLAY(埋まった部分。PLAYと同じ緑)

    const float radius = KNOB_RADIUS * knob_scale_;
    DrawCircleAA(knob_x, pos_.y, radius, 32, GetColor(255, 255, 255), TRUE);
    DrawCircleAA(knob_x, pos_.y, radius, 32, GetColor(0, 0, 0), FALSE, 2.0f);    //outline(縁)
}

void ComponentSlider::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Slider")) {
            //有効/無効(enable/disable)
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            ImGui::Text(u8"value: %.2f (%.2f - %.2f)", value_, min_value_, max_value_);
            ImGui::Text(u8"dragging: %s", dragging_ ? "yes" : "no");

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentSlider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentSlider)
