//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Intro.cpp
//! @brief	Story intro screen, shown before the Main Menu(メインメニューの前に出るストーリー画面)
//---------------------------------------------------------------------------
#include "SniperVsZombie_Intro.h"
#include "ObjectNames.h"
#include "StageConfig.h"
#include "StoryText.h"
#include "HudText.h"
#include "SniperVsZombie_MainMenu.h"
#include <Game/Component/ComponentBlackScreen.h>
#include <algorithm>
#include <cstring>
#include <iterator>

namespace SniperVsZombie {

namespace {
constexpr int   LINE_COUNT       = static_cast<int>(std::size(Story::INTRO_LINES));
constexpr float EMPTY_LINE_PAUSE = 0.5f;    //an empty line waits this much of an interval(空行は間隔のこの割合だけ待つ)
constexpr int   LINE_GAP         = 10;      //extra pixels between lines(行の間の追加ピクセル)
}    // namespace

//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_Intro::Init()
{
    //the scene object is reused when we come back here, so reset everything(戻ってきた時もシーンは使い回されるので全部リセット)
    elapsed_          = 0.0f;
    isLeaving_        = false;
    wasLeftMouseDown_ = true;    //true = a click that is still held from the last scene does not count(true=前のシーンから押しっぱなしのクリックは数えない)

    //BLACK SCREEN TRANSITION
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName(Names::BLACK_SCREEN);
    blackScreen_ = blackScreenObj->AddComponent<ComponentBlackScreen>();
    blackScreen_->StartFadeIn();    //start fully black, fade INTO the intro(真っ黒から始めてイントロへフェード)

    blackScreenHandle_ = LoadGraph("data/Game/UI/black_screen.png");

    return true;
}

float SniperVsZombie_Intro::GetLineStartTime(int index) const
{
    float time = 0.0f;
    for(int i = 0; i < index; ++i) {
        const bool is_empty  = Story::INTRO_LINES[i][0] == '\0';
        time                += Config::INTRO_LINE_INTERVAL * (is_empty ? EMPTY_LINE_PAUSE : 1.0f);
    }
    return time;
}

bool SniperVsZombie_Intro::IsAllShown() const
{
    return elapsed_ >= GetLineStartTime(LINE_COUNT - 1) + Config::INTRO_LINE_FADE;
}

void SniperVsZombie_Intro::Update()
{
    __super::Update();

    elapsed_ += GetDeltaTime();

    int  mouseInput   = GetMouseInput();
    bool leftDown     = (mouseInput & MOUSE_INPUT_LEFT) != 0;
    bool leftClicked  = leftDown && !wasLeftMouseDown_;    //only fire once, on the frame the click starts(クリックした瞬間だけ反応)
    wasLeftMouseDown_ = leftDown;

    //ignore clicks while a fade is running or after we already chose to leave(フェード中や、もう出ると決めた後のクリックは無視)
    if(!leftClicked || isLeaving_ || !blackScreen_->isFullyClear())
        return;

    if(!IsAllShown()) {
        //click 1: show every line now(1回目: 全ての行をすぐ表示)
        elapsed_ = GetLineStartTime(LINE_COUNT - 1) + Config::INTRO_LINE_FADE;
        return;
    }

    //click 2: fade to black first, THEN change scene once the screen is fully black(2回目: 先に暗転し、真っ黒になってからシーン変更)
    isLeaving_ = true;
    blackScreen_->StartFadeOut([]() { Scene::Change(Scene::GetScene<SniperVsZombie_MainMenu>()); });
}

void SniperVsZombie_Intro::PostDraw()
{
    __super::PostDraw();

    //BG(背景)
    DrawGraph(0, 0, blackScreenHandle_, true);

    //STORY TEXT - centered, each line fades in on its own timer(ストーリー文。中央揃え、各行が自分のタイマーでフェードイン)
    const int line_h = Hud::GetLineHeight() + LINE_GAP;
    int       y      = WINDOW_H / 2 - (LINE_COUNT * line_h) / 2;
    const int white  = GetColor(255, 255, 255);
    const int prompt = GetColor(180, 180, 180);

    for(int i = 0; i < LINE_COUNT; ++i) {
        const char* text  = Story::INTRO_LINES[i];
        const float alpha = std::clamp((elapsed_ - GetLineStartTime(i)) / Config::INTRO_LINE_FADE, 0.0f, 1.0f);
        if(alpha > 0.0f && text[0] != '\0') {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * alpha));
            Hud::DrawLabelCenter(WINDOW_W / 2, y, white, "%s", text);
        }
        y += line_h;
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    //PROMPT - tells what the next click does(次のクリックで何が起きるかを表示)
    Hud::DrawLabelCenter(WINDOW_W / 2, WINDOW_H - 70, prompt, "%s", IsAllShown() ? Story::INTRO_PROMPT_CONTINUE : Story::INTRO_PROMPT_SKIP);

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades(最後に描いてフェード中は全てを覆う)
    blackScreen_->DrawBlackScreen();
}

void SniperVsZombie_Intro::Exit()
{
    DeleteGraph(blackScreenHandle_);
    blackScreenHandle_ = -1;
    Hud::ReleaseFont();    //made again next time it is needed(次に必要な時に作り直される)
}

}    // namespace SniperVsZombie
