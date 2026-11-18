//---------------------------------------------------------------------------
//!	@file	ZombieSpawner.h
//! @brief	spawns every zombie type, keeps the spawn timers and alive counts(全ゾンビの出現・出現タイマー・生存数を管理する)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <Game/Component/ComponentEnemyDeath.h>
#include <functional>
#include <memory>
#include <vector>

namespace SniperVsZombie {

//to add a new zombie: 1 value here, 1 case in SpawnZombie(), its lanes/numbers in StageConfig.h(新ゾンビ追加: ここに1つ、SpawnZombieに1case、StageConfig.hにレーンと数値)
enum class ZombieType
{
    Normal,     //walks to the gate and hits it(ゲートまで歩いて攻撃)
    Stalker,    //jumps up the tower and hits the player(塔に跳び上がりプレイヤーを攻撃)
};

//plain helper class owned by MainStage, not an Object. it is stage logic, not something that lives in the world(MainStageが持つ普通のクラス。Objectではない。ワールドにいる物ではなくステージの処理)
class ZombieSpawner
{
public:
    using OnKilledFunc = std::function<void(const ComponentEnemyDeath::KillInfo&)>;

    //back to the start of the stage: no zombies, timers reset(ステージ開始の状態に戻す。ゾンビ0、タイマー初期化)
    void Reset();

    //call once per frame. progress = 0.0 stage start .. 1.0 all civilians saved(毎フレーム1回呼ぶ。progress=0.0開始〜1.0全員救出)
    void Update(float delta_time, float progress);

    //spawn one zombie now. lane = -1 picks a random lane. returns nullptr if a cap is full(今すぐ1体出す。lane=-1でランダム。上限なら nullptr)
    ObjectPtr SpawnZombie(ZombieType type, int lane = -1);

    //what to do when any spawned zombie dies (score, hit marker, effect)(出したゾンビが死んだ時の処理。スコア・ヒットマーカー・エフェクト)
    void SetOnZombieKilled(OnKilledFunc on_killed) { on_zombie_killed_ = on_killed; }

    //called right after a zombie is spawned, e.g. for the first-stalker radio line(ゾンビを出した直後に呼ばれる。最初のストーカーの無線など)
    using OnSpawnedFunc = std::function<void(ZombieType type)>;
    void SetOnZombieSpawned(OnSpawnedFunc on_spawned) { on_zombie_spawned_ = on_spawned; }

    //freeze / unfreeze every zombie in the scene (Object::StatusBit::IsPause), e.g. at the end(シーンの全ゾンビを停止/再開する。終了時など)
    void SetAllPaused(bool paused);

    //zombies still in the scene, dead bodies included until they are removed(シーンに残っているゾンビ数。消えるまでは死体も含む)
    int GetAliveCount() const;
    int GetAliveCount(ZombieType type) const;

    float GetNormalSpawnInterval() const { return normal_spawn_interval_; }

private:
    //forget zombies that were removed from the scene(シーンから消えたゾンビを忘れる)
    void ForgetRemovedZombies();

    bool CanSpawn(ZombieType type) const;

    struct Tracked
    {
        ZombieType            type;
        std::weak_ptr<Object> object;    //weak, the scene owns the zombie, not us(weak。ゾンビの持ち主はシーン)
    };
    std::vector<Tracked> zombies_;

    float normal_spawn_interval_ = 0.0f;    //current interval, shrinks as the stage goes on(今の出現間隔。進行で短くなる)
    float normal_spawn_timer_    = 0.0f;    //time until the next normal zombie(次の通常ゾンビまでの時間)
    float stalker_spawn_timer_   = 0.0f;    //time until the next stalker in phase 2(フェーズ2で次のストーカーまでの時間)

    OnKilledFunc  on_zombie_killed_;
    OnSpawnedFunc on_zombie_spawned_;
};

}    // namespace SniperVsZombie
