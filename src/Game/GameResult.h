//---------------------------------------------------------------------------
//!	@file	GameResult.h
//! @brief	how the stage ended. MainStage sends it to GameOver(ステージの終わり方。MainStageからGameOverへ渡す)
//---------------------------------------------------------------------------
#pragma once

namespace SniperVsZombie {

//each result has its own cutscene and text in GameOver(結果ごとにGameOverのカットシーンと文が違う)
enum class GameResult
{
    Win,             //all civilians evacuated, the heli picks up the sniper(全員避難、ヘリが狙撃手を回収)
    GateBreached,    //the gate HP reached 0(ゲートのHPが0)
    SniperDown,      //the player HP reached 0 (stalker)(プレイヤーのHPが0。ストーカー)
};

}    // namespace SniperVsZombie
