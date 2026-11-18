//---------------------------------------------------------------------------
//!	@file	ComponentSlider.h
//! @brief	a horizontal slider you drag with the mouse(マウスでドラッグする横スライダー)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <Game/Float2.h>
#include <functional>

USING_PTR(ComponentSlider);

//how to use (same idea as ComponentButton)(使い方。ComponentButtonと同じ考え方):
//  slider->Setup({400, 330}, 480, 0.2f, 3.0f, 1.0f)->SetStep(0.05f)->SetOnChanged([](float v) { ... });
//  then call DrawSlider() in the scene's PostDraw(シーンのPostDrawでDrawSliderを呼ぶ)
//hide it with SetStatus(StatusBit::Enable, false): the scene then skips its Update(隠すにはEnableをfalseに。シーンがUpdateを飛ばす)
class ComponentSlider : public Component
{
public:
    BP_COMPONENT_DECL(ComponentSlider, u8"スライダーコンポーネント");

    using OnChangedFunc = std::function<void(float)>;

    //pos = left end of the bar (y = its middle), width = bar length in pixels(pos=バーの左端(yは中央)、width=バーの長さ)
    ComponentSliderPtr Setup(Float2 pos, float width, float min_value, float max_value, float value);

    //snap the value to this step, 0 = no snapping(この単位で値を止める。0=止めない)
    ComponentSliderPtr SetStep(float step);

    //called every time the value changes while dragging(ドラッグで値が変わるたびに呼ばれる)
    ComponentSliderPtr SetOnChanged(OnChangedFunc on_changed);

    //set the value from code, does NOT call OnChanged(コードから値を設定。OnChangedは呼ばない)
    ComponentSliderPtr SetValue(float value);

    float GetValue() const { return value_; }

    bool IsDragging() const { return dragging_; }

    void Init() override;

    void Update() override;

    //draw it. call this manually from the scene's PostDraw(描く。シーンのPostDrawから手動で呼ぶ)
    void DrawSlider();

    void GUI() override;

private:
    float SnapAndClamp(float value) const;

    Float2 pos_       = {0.0f, 0.0f};
    float  width_     = 300.0f;
    float  min_value_ = 0.0f;
    float  max_value_ = 1.0f;
    float  value_     = 0.5f;
    float  step_      = 0.0f;

    OnChangedFunc on_changed_;

    bool  dragging_      = false;
    bool  hovered_       = false;
    bool  was_left_down_ = false;    //to find the frame a click starts(クリックが始まったフレームを見つける用)
    float knob_scale_    = 1.0f;

    static constexpr float BAR_HEIGHT  = 8.0f;     //thickness of the bar(バーの太さ)
    static constexpr float KNOB_RADIUS = 12.0f;    //size of the round handle(丸いつまみの大きさ)
    static constexpr float HOVER_SCALE = 1.25f;    //knob grows on hover, like the buttons(ボタンと同じくホバーでつまみが大きくなる)
    static constexpr float SCALE_SPEED = 10.0f;

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};

CEREAL_CLASS_VERSION(ComponentSlider, 1);
