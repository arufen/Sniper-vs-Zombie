//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Intro.h
//! @brief	Story intro screen, shown before the Main Menu(メインメニューの前に出るストーリー画面)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <Game/Component/ComponentBlackScreen.h>

namespace SniperVsZombie {

//the story lines fade in one by one. click 1 = show all, click 2 = go to the menu(ストーリーの行が1行ずつフェードイン。1回目のクリック=全文表示、2回目=メニューへ)
class SniperVsZombie_Intro : public Scene::Base
{
public:
    BP_CLASS_DECL(SniperVsZombie_Intro, u8"Intro");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

    void Exit() override;

private:
    //time when line i starts to fade in. empty lines are a short pause(i行目がフェードインし始める時間。空行は短い間)
    float GetLineStartTime(int index) const;

    //true when every line is fully visible(全ての行が完全に見えていればtrue)
    bool IsAllShown() const;

    ComponentBlackScreenPtr blackScreen_;    //fade-to-black used when leaving this scene(このシーンを出る時の暗転)
    int                     blackScreenHandle_ = -1;

    float elapsed_          = 0.0f;     //time since the intro started(イントロ開始からの時間)
    bool  isLeaving_        = false;    //true after the last click, waiting for the fade(最後のクリック後。フェード待ち)
    bool  wasLeftMouseDown_ = false;    //used to detect the frame a click starts, not held-down spam(押しっぱなしではなく、クリックした瞬間を見るため)
};

}    // namespace SniperVsZombie
