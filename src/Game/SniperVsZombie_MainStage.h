//---------------------------------------------------------------------------
//!	@file	Tutorial_X.h
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include <System/Scene.h>
#include <Game/Component/ComponentBlackScreen.h>

namespace SniperVsZombie {

class SniperVsZombie_MainStage : public Scene::Base
{
public:
    static constexpr int MAX_ENEMIES = 20;    // total spawn cap so we don't flood ram with infinite enemies

    BP_CLASS_DECL(SniperVsZombie_MainStage, u8"Sniper Main Stage");

    void AddScore(int num);
    void TriggerHitmark(bool isHeadshot);
    int  GetHitEffectHandle() const { return hitEffectHandle_; }

    //! @brief enemy calls this in its own Init(), so the count stays accurate without scanning the scene
    void IncreaseEnemyCount();
    //! @brief enemy calls this in its own Exit(), when it actually leaves the scene
    void DecreaseEnemyCount();

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

    void Exit() override;

private:
    void CreateEnemy();
    void UpdateSpawnTimer();
    void SetIntervalSpawnTime(float newTimer);
    void UpdateCivilian();
    void StageDifficultyUpdate();

    void OnGameOver(bool isWin);

    inline static const float3 ENEMY_DEFAULT_SPAWN          = {394.0f, 41.0f, 440.0f};
    float                      time_to_spawn_enemy_interval = 5.0f;
    float                      time_to_spawn_enemy          = time_to_spawn_enemy_interval;
    int                        score_                       = 0;
    float                      gateHP_                      = 9999.0f;
    float                      gateMaxHP_                   = 9999.0f;
    bool                       isGameOver_                  = false;
    int                        enemiesCurrentCount_         = 0;    // enemies ALIVE right now, capped at MAX_ENEMIES
    int                        civilianSaved_               = 0;
    int                        civilianTotalCount_          = 36;
    float                      civilianSaveTimeInterval_    = 5.0f;    //default 5.0f
    float                      civilianSaveTime_            = civilianSaveTimeInterval_;
    int                        hitMarkerHandle_;
    int                        hitMarkerRedHandle_;
    int                        hitmarkerAlpha_      = 0;
    bool                       hitmarkerIsHeadshot_ = false;
    int                        hitEffectHandle_     = -1;
    bool                       isWin_               = false;    //!< true = all civilians saved, false = gate destroyed

    //GATE ARROW INDICATOR
    float3 gatePos_     = {0.0f, 0.0f, 0.0f};    //!< stores gate world pos so we don't Get<Gate>() every frame just to draw the marker
    float  arrowTimer_  = 0.0f;                  //!< used for sin bounce animation on the arrow
    int    arrowHandle_ = -1;                    //!< data/Game/UI/arrow.png

    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene
};

}    // namespace SniperVsZombie
