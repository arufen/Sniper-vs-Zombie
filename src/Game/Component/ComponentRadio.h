//---------------------------------------------------------------------------
//!	@file	ComponentRadio.h
//! @brief	radio subtitles at the bottom of the screen, one message at a time(画面下の無線字幕。1度に1メッセージ)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <deque>
#include <string>

USING_PTR(ComponentRadio);

//how to use: radio->Say(Story::RADIO_GATE_FIRST_HIT); then call DrawRadio() in the scene's PostDraw(使い方: Sayで話させ、シーンのPostDrawでDrawRadioを呼ぶ)
//the time on screen grows with the text length, see Config::RADIO_*(表示時間は文の長さで伸びる。Config::RADIO_*参照)
class ComponentRadio : public Component
{
public:
    BP_COMPONENT_DECL(ComponentRadio, u8"無線字幕コンポーネント");

    void Init() override;

    void Update() override;

    void Exit() override;

    //add a message to the queue. interrupt = throw away the current + waiting ones first (for endings)(メッセージを追加。interrupt=今のと待ちを捨ててから(終了時用))
    void Say(const char* text, bool interrupt = false);

    //remove everything(全部消す)
    void Clear();

    //true while a message is on screen or waiting(表示中または待ちがある間true)
    bool IsBusy() const { return !queue_.empty() || state_ != State::Idle; }

    //draw the subtitle box. call by hand in the scene's PostDraw, before the black screen(字幕を描く。シーンのPostDrawで黒画面より前に手動で呼ぶ)
    void DrawRadio();

    //how long a text stays on screen, in seconds(文が表示される秒数)
    static float GetShowTime(const char* text);

private:
    enum class State
    {
        Idle,       //nothing on screen(何も表示していない)
        Showing,    //a message is on screen (with fade in/out)(表示中。フェードイン/アウト込み)
        Gap,        //short empty time before the next one(次までの短い空き時間)
    };

    void StartNext();

    std::deque<std::string> queue_;      //waiting messages(待っているメッセージ)
    std::string             current_;    //message on screen(表示中のメッセージ)
    State                   state_      = State::Idle;
    float                   timer_      = 0.0f;    //time in the current state(今の状態の経過時間)
    float                   show_time_  = 0.0f;    //total show time of the current message(今のメッセージの表示時間)
    int                     beep_sound_ = -1;      //[DxLib] radio beep, -1 if the file is missing(無線のピー音。ファイルが無ければ-1)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};

CEREAL_CLASS_VERSION(ComponentRadio, 1);

namespace SniperVsZombie {
//make the Radio object (named Names::RADIO) and return its component. call in a scene's Init(Radio Object(Names::RADIO)を作り、コンポーネントを返す。シーンのInitで呼ぶ)
ComponentRadioPtr CreateRadio();
}    // namespace SniperVsZombie
