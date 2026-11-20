//---------------------------------------------------------------------------
//!	@file	PauseMenu.cpp
//! @brief	in-game pause panel(ゲーム中のポーズパネル)
//---------------------------------------------------------------------------
#include "PauseMenu.h"
#include "GameSettings.h"
#include "HudText.h"

namespace SniperVsZombie {

namespace {
constexpr float BUTTON_W   = 170.0f;
constexpr float BUTTON_H   = 56.0f;
constexpr float BUTTON_GAP = 15.0f;
}    // namespace

void PauseMenu::Create(const Callbacks& callbacks)
{
    objects_.clear();
    open_ = false;

    sliders_.Create(callbacks.on_setting_changed);
    for(const auto& obj : sliders_.GetObjects()) objects_.push_back(obj);

    //3 buttons in one row, centered in the panel(3つのボタンを1行に、パネル中央に並べる)
    const float row_w = BUTTON_W * 3.0f + BUTTON_GAP * 2.0f;
    const float left  = static_cast<float>(Panel::CENTER_X) - row_w * 0.5f;

    auto make = [this](const char* name, int index, float left_x, const char* label, int color, std::function<void()> on_click) {
        auto obj = Scene::Object::Create<Object>();
        obj->SetName(name);
        objects_.push_back(obj);

        auto button = obj->AddComponent<ComponentButton>();
        button->Setup({left_x + (BUTTON_W + BUTTON_GAP) * static_cast<float>(index), Panel::BUTTON_Y}, {BUTTON_W, BUTTON_H}, label, color, on_click);
        return button;
    };

    resume_  = make("PauseResumeButton", 0, left, "RESUME", GetColor(0, 200, 0), callbacks.on_resume);
    restart_ = make("PauseRestartButton", 1, left, "RESTART", GetColor(200, 140, 0), callbacks.on_restart);
    menu_    = make("PauseMenuButton", 2, left, "MENU", GetColor(200, 0, 0), callbacks.on_menu);

    SetOpen(false);
}

void PauseMenu::SetOpen(bool open)
{
    //closing = the player is done with the sliders, keep the values for next time(閉じる=調整完了。次回のために保存)
    if(open_ && !open)
        Settings::Save();
    open_ = open;

    sliders_.SetVisible(open);
    if(open)
        sliders_.SyncFromSettings();

    //hidden buttons skip their Update, so they can't be clicked(隠れたボタンはUpdateしないので押せない)
    for(auto& button : {resume_, restart_, menu_}) {
        if(button)
            button->SetStatus(Component::StatusBit::Enable, open);
    }
}

void PauseMenu::Draw()
{
    if(!open_)
        return;

    SettingsSliders::DrawPanel("ポーズ中");    //EN: Paused
    sliders_.DrawSliders();

    for(auto& button : {resume_, restart_, menu_}) {
        if(button)
            button->DrawButton();
    }

    //key hint under the panel(パネルの下にキーの案内)
    Hud::DrawLabelCenter(Panel::CENTER_X, Panel::BOTTOM + 12, GetColor(200, 200, 200), "P / Tab で再開");    //EN: P / Tab to resume
}

bool PauseMenu::Owns(const Object* obj) const
{
    for(const auto& own : objects_) {
        if(own.get() == obj)
            return true;
    }
    return false;
}

}    // namespace SniperVsZombie
