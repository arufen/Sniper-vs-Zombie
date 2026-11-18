//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_GameOver.h
//! @brief	SniperVsZombie_GameOver
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include "Float2.h"
#include "GameResult.h"
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>
#include <Game/Component/ComponentCameraPath.h>
#include <Game/Component/ComponentRadio.h>

namespace SniperVsZombie {
// forward declaration - a weak_ptr member only needs this, not the full class definition.
// avoids including SniperVsZombie_Helicopter.h here, which would double-include it
// whenever GameOver.cpp includes both this header AND Helicopter.h directly.
class Helicopter;

//plays the ending cutscene for the result, then shows the result text + RETRY / MENU(結果のエンディングを再生し、その後に結果の文字とRETRY/MENUを出す)
class SniperVsZombie_GameOver : public Scene::Base
{
public:
    BP_CLASS_DECL(SniperVsZombie_GameOver, u8"Game Over");

    //! @brief called by MainStage right before Scene::Change, so this scene knows how it ended(MainStageがScene::Change直前に呼ぶ。どう終わったかを知るため)
    void SetResult(GameResult result, int finalScore)
    {
        result_     = result;
        isWin_      = result == GameResult::Win;
        finalScore_ = finalScore;
    }

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

    void Exit() override;

private:
    //one function per ending. each sets the camera shots + the actors(エンディングごとに1関数。カメラショットと登場物を用意する)
    void SetupWin();
    void SetupGateBreached();
    void SetupSniperDown();

    //win cutscene radio lines, timed with the helicopter(勝利カットシーンの無線。ヘリに合わせて流す)
    void UpdateWinRadio();

    //cutscene over: wait RESULT_DELAY, then the result text + buttons(カットシーン終了: RESULT_DELAY待ってから結果とボタン)
    void OnCutsceneFinished();
    void ShowResult();

    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene
    ComponentCameraPathPtr  cutscene_;       //ending camera(エンディングのカメラ)
    ComponentRadioPtr       radio_;          //radio subtitles(無線字幕)

    ComponentButtonPtr retryButton_;    // back to MainStage
    ComponentButtonPtr menuButton_;     // back to MainMenu

    std::weak_ptr<Helicopter> helicopter_;

    GameResult result_     = GameResult::Win;    //which ending to play(どのエンディングを再生するか)
    bool       isWin_      = true;               //!< true = all civilians saved, false = gate destroyed / sniper down
    int        finalScore_ = 0;                  //!< score carried over from MainStage

    //SOUNDS - -1 when the file is missing (RequiredAssets.md)(音。ファイルが無ければ-1)
    int bgm_          = -1;
    int sfxHeli_      = -1;
    int sfxGateBreak_ = -1;

    float resultDelay_      = -1.0f;    //counts down after the cutscene, -1 = not started(カットシーン後のカウントダウン。-1=未開始)
    bool  showResult_       = false;    //true = result text + buttons are visible(true=結果とボタンが見える)
    int   winRadioStep_     = 0;        //which win radio line comes next(次に流す勝利の無線)
    bool  wasLeftMouseDown_ = true;     //for the skip click. true = a held click from the last scene does not count(スキップ用。押しっぱなしは数えない)
};

}    // namespace SniperVsZombie
