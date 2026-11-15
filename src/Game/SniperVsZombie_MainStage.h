//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_MainStage.h
//! @brief	SniperVsZombie_MainStage
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentPlayerStats.h>
#include <Game/Component/ComponentEnemyDeath.h>
#include "ZombieSpawner.h"

namespace SniperVsZombie {

//the stage: builds the map, owns the spawner, tracks civilians / score, decides win or lose, draws the HUD(ステージ: マップ作成、スポナー所持、市民とスコア管理、勝敗判定、HUD描画)
class SniperVsZombie_MainStage : public Scene::Base
{
public:
    BP_CLASS_DECL(SniperVsZombie_MainStage, u8"Sniper Main Stage");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

    void Exit() override;

private:
    //put every value back to "stage start". the engine keeps this scene object alive between plays(全ての値を「ステージ開始」に戻す。エンジンはこのシーンを使い回す)
    void ResetStageState();

    void CreateMap();

    //every zombie death comes here, wired through ZombieSpawner(全ゾンビの死亡はここに来る。ZombieSpawner経由)
    void OnZombieKilled(const ComponentEnemyDeath::KillInfo& kill);

    void PlayHitEffect(const float3& position);
    void TriggerHitmark(bool isHeadshot);
    void AddScore(int num);

    void UpdateCivilian();

    //0.0 = stage start, 1.0 = all civilians saved(0.0=開始、1.0=全員救出)
    float GetStageProgress() const;

    void OnGameOver(bool isWin, bool breakGate);

    ZombieSpawner spawner_;    //all zombie spawning lives here now(ゾンビの出現処理は全部ここ)

    int   score_               = 0;
    float gateHP_              = 9999.0f;
    float gateMaxHP_           = 9999.0f;
    bool  isGameOver_          = false;
    bool  isWin_               = false;    //!< true = all civilians saved, false = gate destroyed / player dead
    int   civilianSaved_       = 0;
    float civilianSaveTime_    = 0.0f;
    int   hitMarkerHandle_     = -1;
    int   hitMarkerRedHandle_  = -1;
    int   hitmarkerAlpha_      = 0;
    bool  hitmarkerIsHeadshot_ = false;
    int   hitEffectHandle_     = -1;

    //GATE ARROW INDICATOR
    float3 gatePos_     = {0.0f, 0.0f, 0.0f};    //!< stores gate world pos so we don't Get<Gate>() every frame just to draw the marker
    float  arrowTimer_  = 0.0f;                  //!< used for sin bounce animation on the arrow
    int    arrowHandle_ = -1;                    //!< data/Game/UI/arrow.png

    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene

    std::weak_ptr<ComponentPlayerStats>
        playerStats_;    //weak ref to player HP, weak so the stage does not keep it alive(プレイヤーHPへの弱参照。ステージが寿命を握らないようweak)
};

}    // namespace SniperVsZombie
