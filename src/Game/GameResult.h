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

//numbers for the result screen, collected by MainStage during play(結果画面用の数値。MainStageがプレイ中に集める)
struct StageStats
{
    int   score           = 0;
    int   kills           = 0;    //zombies killed(倒したゾンビ数)
    int   headshots       = 0;    //kills that were headshots(ヘッドショットだったキル)
    int   shots           = 0;    //bullets fired(撃った弾数)
    int   civilians_saved = 0;
    int   civilians_total = 0;
    float gate_hp_ratio   = 1.0f;    //0.0 = broken, 1.0 = no damage(0.0=壊れた、1.0=無傷)
    float play_time       = 0.0f;    //seconds of real play, no opening, no pause(実際のプレイ秒数。オープニングとポーズは含まない)

    //0..100. a zombie dies from one hit, so every kill is one hit bullet(0〜100。ゾンビは1発で死ぬので、キル=命中した弾)
    int GetHeadshotPercent() const { return kills > 0 ? (headshots * 100 + kills / 2) / kills : 0; }
    int GetAccuracyPercent() const { return shots > 0 ? (kills * 100 + shots / 2) / shots : 0; }
};

}    // namespace SniperVsZombie
