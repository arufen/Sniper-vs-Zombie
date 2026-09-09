//---------------------------------------------------------------------------
//!	@file	Tutorial_X.h
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {

class SniperVsZombie_MainStage : public Scene::Base
{
public:
    static constexpr int MAX_ENEMIES = 50;    // total spawn cap so we don't flood ram with infinite enemies

    BP_CLASS_DECL(SniperVsZombie_MainStage, u8"Sniper Main Stage");

    void AddScore(int num);
    void TriggerHitmark(bool isHeadshot);

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    void Update() override;

    void PostDraw() override;

private:
    void CreateEnemy();
    void UpdateSpawnTimer();
    void SetIntervalSpawnTime(float newTimer);
    void UpdateCivilian();
    void StageDifficultyUpdate();

    void OnGameOver();

    inline static const float3 ENEMY_DEFAULT_SPAWN          = {394.0f, 41.0f, 440.0f};
    float                      time_to_spawn_enemy_interval = 5.0f;
    float                      time_to_spawn_enemy          = time_to_spawn_enemy_interval;
    int                        score_                       = 0;
    float                      gateHP_                      = 9999.0f;
    float                      gateMaxHP_                   = 9999.0f;
    bool                       isGameOver_                  = false;
    int                        totalEnemiesSpawned_         = 0;    // running total, capped at MAX_ENEMIES
    int                        civilianSaved_               = 0;
    int                        civilianTotalCount_          = 36;
    float                      civilianSaveTimeInterval_    = 5.0f;
    float                      civilianSaveTime_            = civilianSaveTimeInterval_;
    int                        hitMarkerHandle_;
    int                        hitMarkerRedHandle_;
    int                        hitmarkerAlpha_      = 0;
    bool                       hitmarkerIsHeadshot_ = false;
};

}    // namespace SniperVsZombie
