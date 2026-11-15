//---------------------------------------------------------------------------
//!	@file	StageConfig.h
//! @brief	all stage tuning numbers, positions and lanes in one place(ステージの調整値・位置・レーンを1か所にまとめる)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <vector>

//balance pass = only edit this file. if a number is used in 2+ scenes, it MUST live here(バランス調整はこのファイルだけ編集する。2つ以上のシーンで使う数値は必ずここに置く)
namespace SniperVsZombie::Config {

//---------------------------------------------------------------------------
// PLAYER(プレイヤー)
//---------------------------------------------------------------------------
inline const float3 PLAYER_SPAWN_POSITION = {447.147f, 231.806f, 585.323f};
inline const float3 PLAYER_SPAWN_ROTATION = {0.0f, -180.0f, 0.0f};
inline const float3 PLAYER_EYE_TO_BODY    = {0.0f, -10.0f, 0.0f};    //hit body sits under the camera eye(当たり判定の体はカメラの目の下)

//---------------------------------------------------------------------------
// MAP(マップ) - shared by MainMenu, MainStage and GameOver(MainMenu・MainStage・GameOverで共有)
//---------------------------------------------------------------------------
inline const float3 GATE_POSITION = {67.726f, 0.911f, 760.512f};
inline const float3 GATE_SCALE    = {0.75f, 0.75f, 0.75f};
inline const float3 GATE_ROTATION = {0.0f, 90.0f, 0.0f};

inline const float3 WOOD_TOWER_POSITION = {437.0f, 100.0f, 550.0f};
inline const float3 WOOD_TOWER_ROTATION = {0.0f, -90.0f, 0.0f};

inline const float3 HELI_POSITION          = {1.360f, 414.269f, 700.566f};
inline const float3 HELI_SCALE             = {0.75f, 0.75f, 0.75f};
inline const float3 HELI_ROTATION_STAGE    = {0.0f, -153.0f, 0.0f};
inline const float3 HELI_ROTATION_CUTSCENE = {0.0f, -90.0f, 0.0f};

//---------------------------------------------------------------------------
// CIVILIANS = the stage timer(市民=ステージのタイマー)
//---------------------------------------------------------------------------
inline constexpr int   CIVILIAN_TOTAL         = 36;      //win when this many are saved. design doc says 20(この人数で勝ち。企画書は20)
inline constexpr float CIVILIAN_SAVE_INTERVAL = 5.0f;    //seconds per saved civilian(1人救出ごとの秒数)

//---------------------------------------------------------------------------
// ZOMBIE SPAWNING(ゾンビの出現)
//---------------------------------------------------------------------------
inline constexpr int MAX_ENEMIES  = 20;    //max zombies alive at once, all types(全種類合わせた同時生存数の上限)
inline constexpr int MAX_STALKERS = 2;     //max stalkers alive at once, also counted in MAX_ENEMIES(ストーカーの同時上限。MAX_ENEMIESにも含む)

//normal zombie spawn interval goes from START (stage begin) to END (all civilians saved)(通常ゾンビの出現間隔。開始時STARTから全員救出時ENDへ)
inline constexpr float NORMAL_SPAWN_INTERVAL_START = 5.0f;
inline constexpr float NORMAL_SPAWN_INTERVAL_END   = 1.0f;
inline constexpr float NORMAL_SPEED_MIN            = 0.1f;
inline constexpr float NORMAL_SPEED_MAX            = 0.3f;

inline constexpr float STALKER_PHASE_START =
    0.5f;    //stage progress (0..1) when stalkers start. 0.5 = half the civilians(ストーカーが出始める進行度。0.5=市民の半分)
inline constexpr float STALKER_SPAWN_INTERVAL = 12.0f;    //seconds between stalkers(ストーカーの出現間隔・秒)
inline constexpr float STALKER_SPEED_MIN      = 0.3f;     //a bit faster than normal zombies(通常ゾンビより少し速い)
inline constexpr float STALKER_SPEED_MAX      = 0.4f;

//---------------------------------------------------------------------------
// LANES(レーン)
//---------------------------------------------------------------------------
//one lane = spawn + waypoints to the gate. kept together so they can not get out of sync(1レーン=出現位置+ゲートまでの経路。セットで持つのでズレない)
struct ZombieLane
{
    float3              spawn;    //spawn position(出現位置)
    std::vector<float3> path;     //waypoints before the target(標的の前のウェイポイント)
};

//stalker lane = a zombie lane + landing points (roof, then tower)(ストーカーのレーン=通常レーン+着地点。屋根→塔)
struct StalkerLane
{
    float3              spawn;    //spawn position(出現位置)
    std::vector<float3> path;     //ground waypoints before the first jump(最初のジャンプ前の地上ウェイポイント)
    std::vector<float3> jumps;    //landing points in order(順番に跳ぶ着地点)
};

inline const std::vector<ZombieLane> NORMAL_LANES = {
    //lane 0
    { {-62.530f, 28.521f, 309.090f}, {{236.747f, 0.139f, 316.820f}, {233.324f, 0.139f, 671.936f}, {70.552f, 0.139f, 726.139f}}},
    //lane 1
    {{-358.739f, 24.193f, 501.069f},                              {{-332.641f, 0.139f, 685.004f}, {64.192f, 0.139f, 719.587f}}},
    //lane 2
    { {460.635f, 18.468f, 707.177f},                               {{254.435f, 0.139f, 678.518f}, {64.192f, 0.139f, 719.587f}}},
};

//TODO: placeholder numbers! pick the real roof/tower spots in game(仮の数値!屋根と塔の位置はゲーム内で決めること)
//landing a little too HIGH is safe, gravity drops it onto the floor. too LOW sinks into the model(着地点は少し高めが安全。重力で床に落ちる。低すぎるとモデルに埋まる)
inline const std::vector<StalkerLane> STALKER_LANES = {
    //lane 0 - Behind the building
    {   {95.206f, 5.112f, 1008.897f},{{270.920f, 5.112f, 996.723f}},{{320.489f, 81.372f, 876.786f}, {357.594f, 83.205f, 786.778f}, {447.0f, 222.0f, 600.0f}}                                },
    //lane 1 - Building to building
    {{-177.774f, 259.039f, 609.537f},
     {{-166.527f, 259.039f, 572.121f}},
     {{-24.335f, 213.863f, 579.770f}, {54.302f, 290.719f, 428.799f}, {158.154f, 186.736f, 561.080f}, {447.0f, 222.0f, 600.0f}}},
};

}    // namespace SniperVsZombie::Config
