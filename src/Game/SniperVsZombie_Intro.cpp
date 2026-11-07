//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Intro.cpp
//! @brief	Story intro screen, shown before the Main Menu
//---------------------------------------------------------------------------
#include "SniperVsZombie_Intro.h"
#include "SniperVsZombie_MainMenu.h"
#include <Game/Component/ComponentBlackScreen.h>

namespace SniperVsZombie {
// one line per array entry - easy to tweak the wording later without touching layout code
//static const char* STORY_LINES[] = {
//	"A zombie outbreak has overtaken the city.",
//	"",
//	"You are stationed at the last checkpoint gate -",
//	"the only route left for survivors to escape.",
//	"",
//	"Civilians are making their way to you.",
//	"Hold the line. Protect them.",
//	"",
//	"A rescue helicopter is inbound...",
//	"but only if the gate still stands when it arrives.",
//};
static const char* STORY_LINES[] = {
    "街はゾンビの大量発生に襲われた。",
    "",
    "あなたは最後の検問所のゲートに配置されている——",
    "生存者が逃げられる、唯一の脱出路だ。",
    "",
    "避難民たちがあなたのもとへ向かっている。",
    "持ちこたえろ。彼らを守れ。",
    "",
    "救助ヘリが向かっている……",
    "だが、それはゲートが持ちこたえていればの話だ。",
};

//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_Intro::Init()
{
    //BLACK SCREEN TRANSITION
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName("BlackScreen");
    blackScreen_ = blackScreenObj->AddComponent<ComponentBlackScreen>();
    blackScreen_->StartFadeIn();    // start fully black, fade INTO the intro

    blackScreenHandle_ = LoadGraph("data/Game/UI/black_screen.png");

    return true;
}

void SniperVsZombie_Intro::Update()
{
    __super::Update();

    int  mouseInput   = GetMouseInput();
    bool leftDown     = (mouseInput & MOUSE_INPUT_LEFT) != 0;
    bool leftClicked  = leftDown && !wasLeftMouseDown_;    // only fire once, on the frame the click starts
    wasLeftMouseDown_ = leftDown;

    if(leftClicked) {
        // fade to black first, THEN change scene once the screen is fully black
        blackScreen_->StartFadeOut([]() { Scene::Change(Scene::GetScene<SniperVsZombie_MainMenu>()); });
    }
}

void SniperVsZombie_Intro::PostDraw()
{
    __super::PostDraw();

    //BG
    DrawGraph(0, 0, blackScreenHandle_, true);

    //STORY TEXT - centered, one line at a time
    SetFontSize(28);
    int color     = GetColor(255, 255, 255);
    int lineCount = static_cast<int>(std::size(STORY_LINES));

    // rough vertical centering: start a bit above screen middle, based on how many lines we have
    int y = WINDOW_H / 2 - (lineCount * 36) / 2;

    for(const char* line : STORY_LINES) {
        int width, height, count;
        GetDrawFormatStringSize(&width, &height, &count, line);
        int x = WINDOW_W / 2 - width / 2;
        DrawFormatString(x, y, color, "%s", line);
        y += 36;
    }

    //PROMPT
    SetFontSize(20);
    {
        const char* prompt = "クリックして続ける";
        int         width, height, count;
        GetDrawFormatStringSize(&width, &height, &count, prompt);
        DrawFormatString(WINDOW_W / 2 - width / 2, WINDOW_H - 60, GetColor(180, 180, 180), "%s", prompt);
    }

    SetFontSize(20);

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades
    blackScreen_->DrawBlackScreen();
}

}    // namespace SniperVsZombie
