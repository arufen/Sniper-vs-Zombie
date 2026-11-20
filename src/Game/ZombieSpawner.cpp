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
    phase_                 = 0;
    normal_spawn_interval_ = Config::SPAWN_PHASES[0].normal_interval_start;
    normal_spawn_timer_    = normal_spawn_interval_;
    stalker_spawn_timer_   = 0.0f;    //0 = the first stalker comes right when its phase starts(0なのでフェーズ開始と同時に1体目が出る)
}

int ZombieSpawner::FindPhase(float progress)
{
    //the last phase whose start we have passed(開始を過ぎた最後のフェーズ)
    int phase = 0;
    for(int i = 0; i < static_cast<int>(Config::SPAWN_PHASES.size()); i++) {
        if(progress >= Config::SPAWN_PHASES[i].start)
            phase = i;
    }
    return phase;
}

void ZombieSpawner::Update(float delta_time, float progress)
{
    ForgetRemovedZombies();

    progress = std::clamp(progress, 0.0f, 1.0f);

    //PHASE - a new phase gives a short breather, then the next wave(フェーズ。新しいフェーズは短い休憩の後に次の波)
    const int new_phase = FindPhase(progress);
    if(new_phase != phase_) {
        phase_               = new_phase;
        normal_spawn_timer_  = std::max(normal_spawn_timer_, Config::PHASE_BREATHER);
        stalker_spawn_timer_ = 0.0f;
        if(on_phase_changed_)
            on_phase_changed_(phase_);
    }
    const Config::SpawnPhase& phase = Config::SPAWN_PHASES[phase_];

    //NORMAL - inside a phase the interval slides from its start value to its end value(通常。フェーズ内で間隔が開始値から終了値へなめらかに変わる)
    const float phase_end  = (phase_ + 1 < static_cast<int>(Config::SPAWN_PHASES.size())) ? Config::SPAWN_PHASES[phase_ + 1].start : 1.0f;
    const float phase_len  = std::max(phase_end - phase.start, 0.0001f);
    const float t          = std::clamp((progress - phase.start) / phase_len, 0.0f, 1.0f);
    normal_spawn_interval_ = phase.normal_interval_start + (phase.normal_interval_end - phase.normal_interval_start) * t;

    if(CanSpawn(ZombieType::Normal)) {
        normal_spawn_timer_ -= delta_time;
        if(normal_spawn_timer_ <= 0.0f) {
            normal_spawn_timer_ = normal_spawn_interval_;
            SpawnZombie(ZombieType::Normal);
        }
    }

    //STALKER - only in phases that have them(ストーカー。出るフェーズのみ)
    if(phase.stalker_interval > 0.0f) {
        stalker_spawn_timer_ -= delta_time;
        if(stalker_spawn_timer_ <= 0.0f) {
            stalker_spawn_timer_ = phase.stalker_interval;
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
            const auto& phase  = Config::SPAWN_PHASES[phase_];
            normal->SetMoveSpeed(GetRandomF(phase.normal_speed_min, phase.normal_speed_max));
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

std::vector<float3> ZombieSpawner::GetLivingPositions(ZombieType type) const
{
    std::vector<float3> positions;
    for(const auto& z : zombies_) {
        if(z.type != type)
            continue;
        auto obj = z.object.lock();
        if(!obj || !obj->GetStatus(Object::StatusBit::Alive))
            continue;
        //a dead body is still Alive until its corpse time ends, so ask the death component too(死体は死体時間が終わるまでAliveのままなので、死亡コンポーネントにも聞く)
        if(auto death = obj->GetComponent<ComponentEnemyDeath>(); death && death->IsDead())
            continue;
        positions.push_back(obj->GetTranslate());
    }
    return positions;
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

    if(type == ZombieType::Stalker && GetAliveCount(ZombieType::Stalker) >= Config::SPAWN_PHASES[phase_].max_stalkers)
        return false;

    return true;
}

}    // namespace SniperVsZombie
