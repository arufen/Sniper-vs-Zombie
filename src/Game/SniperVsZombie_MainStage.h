//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_MainStage.h
//! @brief	SniperVsZombie_MainStage
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentPlayerStats.h>
#include <Game/Component/ComponentEnemyDeath.h>
#include <Game/Component/ComponentCameraPath.h>
#include <Game/Component/ComponentRadio.h>
#include "ZombieSpawner.h"
#include "GameResult.h"

namespace SniperVsZombie {

//what the stage is doing right now(ステージが今何をしているか)
enum class StageState
{
    Opening,    //cutscene camera, no zombies, no control(カットシーン。ゾンビなし、操作なし)
    Playing,    //normal game(通常のゲーム)
    Ending,     //won or lost, short hold before the fade(勝ち/負けが決まり、フェード前の短い待ち)
};

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

    //OPENING: camera shots O1 -> O2 -> O3, then the player gets control(オープニング: O1→O2→O3の後、プレイヤーが操作できる)
    void StartOpening();
    void OnOpeningFinished();

    //true = the player can look and shoot, and sees the gun(true=視点操作・射撃ができ、銃が見える)
    void SetPlayerControl(bool enable);

    //true for one frame when the left mouse button goes down(左クリックを押した瞬間の1フレームだけtrue)
    bool IsClickStarted();

    //every zombie death comes here, wired through ZombieSpawner(全ゾンビの死亡はここに来る。ZombieSpawner経由)
    void OnZombieKilled(const ComponentEnemyDeath::KillInfo& kill);

    void PlayHitEffect(const float3& position);
    void TriggerHitmark(bool isHeadshot);
    void AddScore(int num);

    void UpdateCivilian();

    //0.0 = stage start, 1.0 = all civilians saved(0.0=開始、1.0=全員救出)
    float GetStageProgress() const;

    //won or lost: freeze the zombies, say the last radio line, then fade after ENDING_HOLD_TIME(勝敗決定: ゾンビを止め、最後の無線を流し、ENDING_HOLD_TIME後にフェード)
    void OnGameOver(GameResult result);
    void UpdateEnding();

    //STORY BEATS - radio lines when something happens for the first time(ストーリーの節目。初めて何かが起きた時の無線)
    void OnGateDamaged(float hp, float max_hp);
    void OnZombieSpawned(ZombieType type);
    void UpdatePlayerRadio();

    //each line is said only once per run(各無線は1回のプレイで1度だけ)
    struct SaidFlags
    {
        bool gate_hit        = false;
        bool gate_half       = false;
        bool gate_low        = false;
        bool stalker         = false;
        bool civilian_half   = false;
        bool civilian_almost = false;
        bool player_hit      = false;
        bool player_low      = false;
    };
    SaidFlags said_;
    float     prevPlayerHP_      = -1.0f;    //player HP last frame, -1 = not read yet(前フレームのプレイヤーHP。-1=未取得)
    float     endingTimer_       = 0.0f;     //time left before the fade out(フェードアウトまでの残り時間)
    bool      endingFadeStarted_ = false;

    //SOUNDS - -1 when the file is missing, then they are just silent (RequiredAssets.md)(音。ファイルが無ければ-1で無音)
    int   bgm_            = -1;
    int   sfxGateHit_     = -1;
    int   sfxGateBreak_   = -1;
    int   sfxZombieGroan_ = -1;
    int   sfxStalker_     = -1;
    int   sfxPlayerHurt_  = -1;
    float groanCooldown_  = 0.0f;    //time until the next groan is allowed(次のうめき声までの時間)

    ZombieSpawner spawner_;    //all zombie spawning lives here now(ゾンビの出現処理は全部ここ)

    StageState             stageState_ = StageState::Opening;
    ComponentCameraPathPtr opening_;         //opening cutscene camera(オープニングのカットシーンカメラ)
    ComponentRadioPtr      radio_;           //radio subtitles(無線字幕)
    std::weak_ptr<Object>  playerCamera_;    //the FPS camera object (look + shoot + gun)(FPSカメラObject。視点・射撃・銃)
    bool wasLeftMouseDown_ = true;           //for IsClickStarted. true = a held click from the last scene does not count(押しっぱなしのクリックは数えない)

    int        score_               = 0;
    float      gateHP_              = 9999.0f;
    float      gateMaxHP_           = 9999.0f;
    bool       isGameOver_          = false;
    bool       isWin_               = false;              //!< true = all civilians saved, false = gate destroyed / player dead
    GameResult result_              = GameResult::Win;    //how the stage ended, valid when isGameOver_(ステージの終わり方。isGameOver_の時に有効)
    int        civilianSaved_       = 0;
    float      civilianSaveTime_    = 0.0f;
    int        hitMarkerHandle_     = -1;
    int        hitMarkerRedHandle_  = -1;
    int        hitmarkerAlpha_      = 0;
    bool       hitmarkerIsHeadshot_ = false;
    int        hitEffectHandle_     = -1;

    //GATE ARROW INDICATOR
    float3 gatePos_     = {0.0f, 0.0f, 0.0f};    //!< stores gate world pos so we don't Get<Gate>() every frame just to draw the marker
    float  arrowTimer_  = 0.0f;                  //!< used for sin bounce animation on the arrow
    int    arrowHandle_ = -1;                    //!< data/Game/UI/arrow.png

    ComponentBlackScreenPtr blackScreen_;    // fade-to-black used when leaving this scene

    std::weak_ptr<ComponentPlayerStats>
        playerStats_;    //weak ref to player HP, weak so the stage does not keep it alive(プレイヤーHPへの弱参照。ステージが寿命を握らないようweak)
};

}    // namespace SniperVsZombie
