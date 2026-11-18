//---------------------------------------------------------------------------
//!	@file	ZombieSpawner.cpp
//! @brief	ZombieSpawner
//---------------------------------------------------------------------------
#include "ZombieSpawner.h"
#include "StageConfig.h"
#include "SniperVsZombie_NormalZombie.h"
#include "SniperVsZombie_Stalker.h"
#include "Random.h"
#include <algorithm>

namespace SniperVsZombie {

namespace {
//random lane index, or the given one if it is valid(ランダムなレーン番号。正しい番号が渡されたらそれを使う)
int PickLane(int lane, size_t lane_count)
{
    if(lane >= 0 && lane < static_cast<int>(lane_count))
        return lane;
    return rand() % static_cast<int>(lane_count);
}
}    // namespace

void ZombieSpawner::Reset()
{
    zombies_.clear();
    normal_spawn_interval_ = Config::NORMAL_SPAWN_INTERVAL_START;
    normal_spawn_timer_    = Config::NORMAL_SPAWN_INTERVAL_START;
    stalker_spawn_timer_   = 0.0f;    //0 = the first stalker comes right when phase 2 starts(0なのでフェーズ2開始と同時に1体目が出る)
}

void ZombieSpawner::Update(float delta_time, float progress)
{
    ForgetRemovedZombies();

    progress = std::clamp(progress, 0.0f, 1.0f);

    //NORMAL - the interval shrinks from START to END as civilians are saved(通常。市民救出が進むと間隔がSTARTからENDへ縮む)
    normal_spawn_interval_ = Config::NORMAL_SPAWN_INTERVAL_START + (Config::NORMAL_SPAWN_INTERVAL_END - Config::NORMAL_SPAWN_INTERVAL_START) * progress;

    if(CanSpawn(ZombieType::Normal)) {
        normal_spawn_timer_ -= delta_time;
        if(normal_spawn_timer_ <= 0.0f) {
            normal_spawn_timer_ = normal_spawn_interval_;
            SpawnZombie(ZombieType::Normal);
        }
    }

    //STALKER - phase 2 only(ストーカー。フェーズ2のみ)
    if(progress >= Config::STALKER_PHASE_START) {
        stalker_spawn_timer_ -= delta_time;
        if(stalker_spawn_timer_ <= 0.0f) {
            stalker_spawn_timer_ = Config::STALKER_SPAWN_INTERVAL;
            SpawnZombie(ZombieType::Stalker);
        }
    }
}

ObjectPtr ZombieSpawner::SpawnZombie(ZombieType type, int lane)
{
    if(!CanSpawn(type))
        return nullptr;

    ObjectPtr zombie;

    switch(type) {
    case ZombieType::Normal:
        {
            const auto& l      = Config::NORMAL_LANES[PickLane(lane, Config::NORMAL_LANES.size())];
            auto        normal = Scene::Object::Create<NormalZombie>();
            normal->SetMoveSpeed(GetRandomF(Config::NORMAL_SPEED_MIN, Config::NORMAL_SPEED_MAX));
            normal->SetTranslate(l.spawn);
            normal->SetPath(l.path);
            zombie = normal;
            break;
        }

    case ZombieType::Stalker:
        {
            const auto& l       = Config::STALKER_LANES[PickLane(lane, Config::STALKER_LANES.size())];
            auto        stalker = Scene::Object::Create<Stalker>();
            stalker->SetMoveSpeed(GetRandomF(Config::STALKER_SPEED_MIN, Config::STALKER_SPEED_MAX));
            stalker->SetTranslate(l.spawn);
            stalker->SetPath(l.path);
            stalker->SetJumpPoints(l.jumps);
            zombie = stalker;
            break;
        }
    }

    if(!zombie)
        return nullptr;

    //every zombie reports its death the same way, so the stage only wires it once, here(全ゾンビが同じ方法で死亡を伝える。ステージはここで1回繋ぐだけ)
    if(auto death = zombie->GetComponent<ComponentEnemyDeath>())
        death->SetOnKilled(on_zombie_killed_);

    zombies_.push_back({type, zombie});

    if(on_zombie_spawned_)
        on_zombie_spawned_(type);

    return zombie;
}

void ZombieSpawner::SetAllPaused(bool paused)
{
    //IsPause blocks the Update of the zombie and all its components (walk, attack, jump). drawing keeps going(IsPauseはゾンビと全コンポーネントのUpdateを止める。描画は続く)
    for(const auto& z : zombies_) {
        if(auto obj = z.object.lock())
            obj->SetStatus(Object::StatusBit::IsPause, paused);
    }
}

int ZombieSpawner::GetAliveCount() const
{
    int count = 0;
    for(const auto& z : zombies_) {
        if(auto obj = z.object.lock(); obj && obj->GetStatus(Object::StatusBit::Alive))
            count++;
    }
    return count;
}

int ZombieSpawner::GetAliveCount(ZombieType type) const
{
    int count = 0;
    for(const auto& z : zombies_) {
        if(z.type != type)
            continue;
        if(auto obj = z.object.lock(); obj && obj->GetStatus(Object::StatusBit::Alive))
            count++;
    }
    return count;
}

void ZombieSpawner::ForgetRemovedZombies()
{
    //Scene::Object::Release() turns Alive off, so that is "gone" for us too(Release()でAliveがOFFになる。それを「消えた」とみなす)
    std::erase_if(zombies_, [](const Tracked& z) {
        auto obj = z.object.lock();
        return !obj || !obj->GetStatus(Object::StatusBit::Alive);
    });
}

bool ZombieSpawner::CanSpawn(ZombieType type) const
{
    if(GetAliveCount() >= Config::MAX_ENEMIES)
        return false;

    if(type == ZombieType::Stalker && GetAliveCount(ZombieType::Stalker) >= Config::MAX_STALKERS)
        return false;

    return true;
}

}    // namespace SniperVsZombie
