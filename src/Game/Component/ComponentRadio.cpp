//---------------------------------------------------------------------------
//!	@file	ComponentRadio.cpp
//! @brief	radio subtitles(無線字幕)
//---------------------------------------------------------------------------
#include <Game/Component/ComponentRadio.h>
#include <Game/StageConfig.h>
#include <Game/StoryText.h>
#include <Game/AssetPaths.h>
#include <Game/ObjectNames.h>
#include <Game/HudText.h>
#include <Game/SoundUtil.h>
#include <algorithm>

using namespace SniperVsZombie;

namespace {
constexpr int   BOX_MIN_WIDTH = 520;    //subtitle box never gets thinner than this(字幕ボックスの最小幅)
constexpr int   BOX_PADDING   = 24;     //space around the text(文字の周りの余白)
constexpr int   BOX_BOTTOM    = 90;     //distance from the screen bottom(画面下からの距離)
constexpr int   BOX_ALPHA     = 170;    //box darkness at full fade(フェード最大時の箱の濃さ)
constexpr float BEEP_VOLUME   = 0.6f;
}    // namespace

void ComponentRadio::Init()
{
    __super::Init();

    beep_sound_ = Sound::Load(Assets::SFX_RADIO_BEEP);
}

float ComponentRadio::GetShowTime(const char* text)
{
    const float by_length = static_cast<float>(Hud::CountCharacters(text)) / Config::RADIO_CHARS_PER_SECOND;
    return std::max(Config::RADIO_MIN_TIME, by_length) +
           Config::RADIO_FADE_TIME * 2.0f;    //+ fades so the text is fully readable for the whole time(フェード分を足して、全時間しっかり読めるように)
}

void ComponentRadio::Say(const char* text, bool interrupt)
{
    if(interrupt)
        Clear();

    queue_.push_back(text);

    if(state_ == State::Idle)
        StartNext();
}

void ComponentRadio::Clear()
{
    queue_.clear();
    current_.clear();
    state_ = State::Idle;
    timer_ = 0.0f;
}

void ComponentRadio::StartNext()
{
    if(queue_.empty()) {
        state_ = State::Idle;
        return;
    }

    current_ = queue_.front();
    queue_.pop_front();
    show_time_ = GetShowTime(current_.c_str());
    timer_     = 0.0f;
    state_     = State::Showing;

    Sound::Play(beep_sound_, BEEP_VOLUME);
}

void ComponentRadio::Update()
{
    __super::Update();

    if(state_ == State::Idle)
        return;

    timer_ += GetDeltaTime();

    if(state_ == State::Showing && timer_ >= show_time_) {
        state_ = State::Gap;
        timer_ = 0.0f;
    }
    else if(state_ == State::Gap && timer_ >= Config::RADIO_GAP_TIME) {
        StartNext();
    }
}

void ComponentRadio::DrawRadio()
{
    if(state_ != State::Showing)
        return;

    //fade: 0 -> 1 at the start, 1 -> 0 at the end(フェード: 最初に0→1、最後に1→0)
    const float fade_in  = std::clamp(timer_ / Config::RADIO_FADE_TIME, 0.0f, 1.0f);
    const float fade_out = std::clamp((show_time_ - timer_) / Config::RADIO_FADE_TIME, 0.0f, 1.0f);
    const float fade     = std::min(fade_in, fade_out);

    const int line     = Hud::GetLineHeight();
    const int text_w   = Hud::GetTextWidth(current_.c_str());
    const int box_w    = std::max(BOX_MIN_WIDTH, text_w + BOX_PADDING * 2);
    const int box_h    = line * 2 + BOX_PADDING;
    const int center_x = WINDOW_W / 2;
    const int box_x1   = center_x - box_w / 2;
    const int box_y1   = WINDOW_H - BOX_BOTTOM - box_h;

    //dark box(暗い箱)
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(BOX_ALPHA * fade));
    DrawBox(box_x1, box_y1, box_x1 + box_w, box_y1 + box_h, GetColor(0, 0, 0), TRUE);

    //tag (green, left) + message (white, center)(タグ(緑・左)+メッセージ(白・中央))
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * fade));
    Hud::DrawLabel(box_x1 + BOX_PADDING / 2, box_y1 + BOX_PADDING / 2, GetColor(90, 255, 120), "%s", Story::RADIO_TAG);
    Hud::DrawLabelCenter(center_x, box_y1 + BOX_PADDING / 2 + line, GetColor(255, 255, 255), "%s", current_.c_str());
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ComponentRadio::Exit()
{
    Sound::Release(beep_sound_);

    __super::Exit();
}

CEREAL_REGISTER_TYPE(ComponentRadio)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentRadio)

namespace SniperVsZombie {
ComponentRadioPtr CreateRadio()
{
    auto radio_obj = Scene::Object::Create<Object>();
    radio_obj->SetName(Names::RADIO);
    return radio_obj->AddComponent<ComponentRadio>();
}
}    // namespace SniperVsZombie
