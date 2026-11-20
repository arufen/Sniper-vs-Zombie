//---------------------------------------------------------------------------
//!	@file	SettingsSliders.cpp
//! @brief	the settings panel shared by the main menu and the pause menu(メインメニューとポーズメニューで共有する設定パネル)
//---------------------------------------------------------------------------
#include "SettingsSliders.h"
#include "GameSettings.h"
#include "HudText.h"

namespace SniperVsZombie {

namespace {
constexpr float SLIDER_X     = 400.0f;    //left end of every bar(各バーの左端)
constexpr float SLIDER_WIDTH = 480.0f;
constexpr float FIRST_ROW_Y  = 215.0f;    //middle of the first bar(1本目のバーの中央)
constexpr float ROW_GAP      = 105.0f;    //distance between 2 bars(バーの間隔)
constexpr int   LABEL_ABOVE  = 48;        //label sits this far above its bar(名前はバーのこの距離上)

float RowY(int row)
{
    return FIRST_ROW_Y + ROW_GAP * static_cast<float>(row);
}
}    // namespace

void SettingsSliders::Create(OnChangedFunc on_changed)
{
    objects_.clear();

    //one object per slider, same as the menu buttons(ボタンと同じく1スライダー1Object)
    auto make = [this](const char* name, int row, float min_v, float max_v, float value, float step, std::function<void(float)> store) {
        auto obj = Scene::Object::Create<Object>();
        obj->SetName(name);
        objects_.push_back(obj);

        auto slider = obj->AddComponent<ComponentSlider>();
        slider->Setup({SLIDER_X, RowY(row)}, SLIDER_WIDTH, min_v, max_v, value)->SetStep(step)->SetOnChanged(store);
        return slider;
    };

    sensitivity_ = make("SensitivitySlider",
                        0,
                        Settings::MOUSE_SENSITIVITY_MIN,
                        Settings::MOUSE_SENSITIVITY_MAX,
                        Settings::GetMouseSensitivity(),
                        Settings::MOUSE_SENSITIVITY_STEP,
                        [on_changed](float v) {
                            Settings::SetMouseSensitivity(v);
                            if(on_changed)
                                on_changed(Kind::Sensitivity, v);
                        });

    bgm_ = make("BgmSlider", 1, Settings::VOLUME_MIN, Settings::VOLUME_MAX, Settings::GetBgmVolume(), Settings::VOLUME_STEP, [on_changed](float v) {
        Settings::SetBgmVolume(v);
        if(on_changed)
            on_changed(Kind::Bgm, v);
    });

    sfx_ = make("SfxSlider", 2, Settings::VOLUME_MIN, Settings::VOLUME_MAX, Settings::GetSfxVolume(), Settings::VOLUME_STEP, [on_changed](float v) {
        Settings::SetSfxVolume(v);
        if(on_changed)
            on_changed(Kind::Sfx, v);
    });
}

void SettingsSliders::SetVisible(bool visible)
{
    //Enable = false makes the scene skip the component's Update, so hidden sliders can't be dragged(Enable=falseでUpdateが飛ばされ、隠れたスライダーは動かせない)
    for(auto& slider : {sensitivity_, bgm_, sfx_}) {
        if(slider)
            slider->SetStatus(Component::StatusBit::Enable, visible);
    }
}

void SettingsSliders::SyncFromSettings()
{
    if(sensitivity_)
        sensitivity_->SetValue(Settings::GetMouseSensitivity());
    if(bgm_)
        bgm_->SetValue(Settings::GetBgmVolume());
    if(sfx_)
        sfx_->SetValue(Settings::GetSfxVolume());
}

void SettingsSliders::DrawPanel(const char* title)
{
    //dim everything behind the panel(パネルの後ろを全部暗くする)
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
    DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawBox(Panel::LEFT, Panel::TOP, Panel::RIGHT, Panel::BOTTOM, GetColor(25, 25, 30), TRUE);
    DrawBox(Panel::LEFT, Panel::TOP, Panel::RIGHT, Panel::BOTTOM, GetColor(0, 200, 0), FALSE);

    Hud::DrawLabelCenter(Panel::CENTER_X, Panel::TOP + 20, GetColor(255, 255, 255), "%s", title);
}

void SettingsSliders::DrawSliders()
{
    const unsigned int white   = GetColor(255, 255, 255);
    const unsigned int gray    = GetColor(170, 170, 170);
    const int          left_x  = static_cast<int>(SLIDER_X);
    const int          right_x = static_cast<int>(SLIDER_X + SLIDER_WIDTH);

    //SENSITIVITY(マウス感度)
    if(sensitivity_) {
        const int y = static_cast<int>(RowY(0));
        Hud::DrawLabel(left_x, y - LABEL_ABOVE, white, "マウス感度");    //EN: Mouse sensitivity
        Hud::DrawLabelRight(right_x, y - LABEL_ABOVE, white, "%.2fx", sensitivity_->GetValue());
        sensitivity_->DrawSlider();
        Hud::DrawLabelCenter(Panel::CENTER_X, y + 20, gray, "スコープ中は自動で遅くなります");    //EN: It slows down automatically while scoped
    }

    //VOLUMES shown as 0-100 %(音量は0〜100%で表示)
    if(bgm_) {
        const int y = static_cast<int>(RowY(1));
        Hud::DrawLabel(left_x, y - LABEL_ABOVE, white, "BGM音量");    //EN: Music volume
        Hud::DrawLabelRight(right_x, y - LABEL_ABOVE, white, "%d%%", static_cast<int>(bgm_->GetValue() * 100.0f + 0.5f));
        bgm_->DrawSlider();
    }
    if(sfx_) {
        const int y = static_cast<int>(RowY(2));
        Hud::DrawLabel(left_x, y - LABEL_ABOVE, white, "効果音量");    //EN: Sound effect volume
        Hud::DrawLabelRight(right_x, y - LABEL_ABOVE, white, "%d%%", static_cast<int>(sfx_->GetValue() * 100.0f + 0.5f));
        sfx_->DrawSlider();
    }
}

}    // namespace SniperVsZombie
