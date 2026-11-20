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

//SHOT FEEL(撃った時の手応え)
inline constexpr float RECOIL_PITCH =
    1.5f;    //degrees the view kicks up per shot. view only, the aim stays(1発で視点が上に跳ねる角度。見た目だけで照準は動かない)
inline constexpr float RECOIL_RETURN_SPEED = 9.0f;     //how fast the kick goes back, bigger = faster(跳ねが戻る速さ。大きいほど速い)
inline constexpr float GUN_KICK_DISTANCE   = 1.5f;     //how far the gun model moves back per shot(1発で銃モデルが後ろに下がる距離)
inline constexpr float POPUP_TIME          = 1.0f;     //seconds a score popup stays(スコア表示が残る秒数)
inline constexpr float POPUP_RISE          = 45.0f;    //pixels the popup floats up(スコア表示が浮き上がるピクセル数)
inline constexpr float SFX_KILL_VOLUME     = 0.7f;     //kill confirm / headshot sound(キル確認・ヘッドショット音)

//SHOOTING(射撃)
inline constexpr float SHOT_INTERVAL         = 1.0f;       //seconds between 2 shots = bolt action time(2発の間の秒数=ボルトを引く時間)
inline constexpr float SHOT_WALL_CHECK_RANGE = 3000.0f;    //how far a shot looks for a building in its way(弾が進路上の建物を探す距離)
inline constexpr float SHOT_WALL_TOLERANCE =
    20.0f;    //an enemy this close in front of the wall still gets hit (feet vs head height)(壁のこの距離手前までの敵には当たる。足元と頭の高さの差)

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
inline constexpr int MAX_ENEMIES = 20;    //max zombies alive at once, all types(全種類合わせた同時生存数の上限)

//the stage is split into 3 phases by stage progress (civilians saved / total). 36 civilians x 5s = about 60s per phase(ステージは進行度(救出数/総数)で3フェーズに分かれる。36人×5秒=1フェーズ約60秒)
//1 = WARM-UP: slow zombies, time to learn aiming(1=準備: 遅いゾンビ、狙いを覚える時間)
//2 = ESCALATION: stalkers start, more zombies(2=激化: ストーカー登場、ゾンビ増加)
//3 = FINAL DEFENSE: the big push before the last civilians are out(3=最終防衛: 最後の市民が出るまでの大攻勢)
struct SpawnPhase
{
    float start;                    //stage progress 0..1 when this phase begins(このフェーズが始まる進行度0〜1)
    float normal_interval_start;    //seconds between normal zombies at the phase start(フェーズ開始時の通常ゾンビ出現間隔・秒)
    float normal_interval_end;      //... and at the phase end, it slides in between(フェーズ終了時の間隔。間はなめらかに変わる)
    float normal_speed_min;         //walk speed range of normal zombies(通常ゾンビの歩く速さの範囲)
    float normal_speed_max;
    float stalker_interval;    //seconds between stalkers, 0 = no stalkers(ストーカーの出現間隔・秒。0=出ない)
    int   max_stalkers;        //stalkers alive at once, also counted in MAX_ENEMIES(ストーカーの同時上限。MAX_ENEMIESにも含む)
};
inline const std::vector<SpawnPhase> SPAWN_PHASES = {
    //start  interval      speed        stalker
    {       0.0f, 5.0f, 3.5f, 0.10f, 0.20f,  0.0f, 0}, //1 WARM-UP(準備)
    {1.0f / 3.0f, 3.5f, 2.0f, 0.15f, 0.25f, 14.0f, 1}, //2 ESCALATION(激化)
    {2.0f / 3.0f, 2.0f, 1.2f, 0.20f, 0.30f, 10.0f, 2}, //3 FINAL DEFENSE(最終防衛)
};
inline constexpr float PHASE_BREATHER =
    4.0f;    //seconds with no new normal zombie when a phase starts, so the player notices the change(フェーズ開始時に通常ゾンビが出ない秒数。変化に気づけるように)

inline constexpr float STALKER_SPEED_MIN = 0.3f;    //a bit faster than normal zombies(通常ゾンビより少し速い)
inline constexpr float STALKER_SPEED_MAX = 0.4f;

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

//---------------------------------------------------------------------------
// STORY / CUTSCENES(ストーリー・カットシーン)
// positions came from the CameraProbe (F8), see AI Context/PositionsCollections.md(位置はCameraProbe(F8)で取得。PositionsCollections.md参照)
//---------------------------------------------------------------------------

//one camera shot = blend from A to B (eye = camera position, target = where it looks)(1ショット=AからBへブレンド。eye=カメラ位置、target=注視点)
struct CameraShot
{
    float3 eye_a;
    float3 target_a;
    float3 eye_b;
    float3 target_b;
    float  duration;    //seconds(秒)
};

inline constexpr float CUT_FADE_TIME =
    1.0f;    //fade between 2 shots, total. half at the end of a shot, half at the start of the next(ショット間のフェード合計。半分を前ショットの最後、半分を次の最初に重ねる)

//INTRO(イントロ)
inline constexpr float INTRO_LINE_INTERVAL = 1.2f;    //seconds between 2 story lines appearing(ストーリー行が出る間隔・秒)
inline constexpr float INTRO_LINE_FADE     = 0.6f;    //fade-in time of one line(1行のフェードイン時間)

//OPENING (MainStage)(オープニング)
inline const std::vector<CameraShot> OPENING_SHOTS = {
    //O1: look at the ground, tilt up to the helicopter(O1: 地面を見て、ヘリへ見上げる)
    {{420.526f, 303.759f, 445.839f}, {345.379f, 267.124f, 500.709f}, {420.526f, 303.759f, 445.839f}, {338.548f, 310.346f, 502.726f}, 4.0f},
    //O2: push forward to the gate(O2: ゲートへ前進)
    {{205.136f, 135.576f, 651.688f},  {141.357f, 76.477f, 701.079f},  {114.261f, 68.438f, 669.705f},   {73.484f, 21.526f, 748.041f}, 4.0f},
    //O3: fly into the player's eyes (B = P0)(O3: プレイヤーの目へ飛ぶ。B=P0)
    {{464.041f, 285.927f, 456.949f}, {419.590f, 233.830f, 529.818f}, {447.147f, 233.306f, 585.323f}, {357.566f, 197.144f, 611.160f}, 4.0f},
};
inline constexpr float PLAYER_START_YAW   = -73.91f;    //P0 look, from CameraProbe "look"(P0の視線。CameraProbeの"look")
inline constexpr float PLAYER_START_PITCH = 21.20f;

//RADIO(無線)
inline constexpr float RADIO_MIN_TIME = 2.0f;    //shortest time a message stays(メッセージの最短表示時間)
inline constexpr float RADIO_CHARS_PER_SECOND =
    5.0f;                                          //longer text stays longer: time = max(MIN, chars / this)(長い文ほど長く表示: 時間=max(MIN, 文字数/これ))
inline constexpr float RADIO_FADE_TIME = 0.25f;    //fade in/out of the subtitle box(字幕ボックスのフェード)
inline constexpr float RADIO_GAP_TIME  = 0.3f;     //empty time between 2 messages(2つのメッセージの間の空き時間)

//STORY BEATS(ストーリーの節目)
inline constexpr float GATE_WARN_HALF        = 0.5f;      //gate HP ratio for the "half" warning(「半分」警告のゲートHP割合)
inline constexpr float GATE_WARN_LOW         = 0.25f;     //gate HP ratio for the "low" warning(「残りわずか」警告のゲートHP割合)
inline constexpr float PLAYER_WARN_LOW       = 0.3f;      //player HP ratio for the "badly hurt" line(「重傷」の無線のプレイヤーHP割合)
inline constexpr int   CIVILIAN_ALMOST_LEFT  = 3;         //"almost everyone" when this many are left(残りこの人数で「あと少し」)
inline constexpr float ENDING_HOLD_TIME      = 2.0f;      //MainStage: time between the end and the fade out(MainStage: 終了からフェードアウトまでの時間)
inline constexpr float SNIPER_DOWN_RED_ALPHA = 140.0f;    //how red the screen gets when the sniper is down(狙撃手ダウン時の画面の赤さ)

//WIN: helicopter extraction (GameOver)(勝ち: ヘリで脱出)
//the heli is already waiting next to the tower (it evacuated the civilians from there), so it starts at W2(ヘリは既に塔の横で待っている(そこから市民を避難させた)ので、W2から始まる)
inline const float3    HELI_WIN_HOVER    = {-34.072f, 432.310f, 716.091f};    //W2
inline const float3    HELI_WIN_LEAVE    = {646.338f, 705.514f, -24.993f};    //W3
inline constexpr float HELI_HOVER_TIME   = 3.0f;    //hover at W2 = the sniper gets in. the 2 times add up to 6.0(W2でホバリング=狙撃手が乗る。2つの合計が6.0)
inline constexpr float HELI_FLY_OUT_TIME = 3.0f;    //W2 -> W3
//the model's nose points to its local -X, so +90 makes it face the travel direction (from the old cutscene: yaw -90 flew toward -Z)(モデルの機首はローカル-X向きなので+90で進行方向を向く。旧カットシーン: ヨー-90で-Z方向に飛んでいた)
inline constexpr float HELI_MODEL_YAW_OFFSET = 90.0f;
inline constexpr float HELI_HOVER_BOB        = 3.0f;                              //up/down wobble while hovering(ホバリング中の上下の揺れ)
inline const float3    WIN_CAMERA_EYE_START  = {522.771f, 425.551f, 766.159f};    //C1
inline const float3    WIN_CAMERA_EYE_END    = {977.128f, 520.000f, 726.781f};    //C2

//LOSE: sniper down (GameOver)(負け: 狙撃手ダウン)
inline const CameraShot SNIPER_DOWN_SHOT = {
    {538.754f, 353.812f, 452.773f},
    {504.198f, 286.260f, 517.908f},
    {505.555f, 290.313f, 513.613f},
    {470.609f, 222.761f, 578.539f},
    1.0f
};    //S10 -> S11

//LOSE: gate breached (GameOver)(負け: ゲート突破)
inline const CameraShot GATE_BREACH_SHOT = {
    {69.047f, 150.509f, 654.136f},
    {68.923f,  66.708f, 708.703f},
    {68.773f,  65.944f, 673.842f},
    {69.079f,  17.860f, 761.522f},
    2.0f
};    //L10 -> L11

inline constexpr float RESULT_DELAY =
    0.5f;    //GameOver: wait after the cutscene before the result + buttons show(カットシーン後、結果とボタンが出るまでの待ち)

//---------------------------------------------------------------------------
// AUDIO VOLUMES 0.0 - 1.0(音量)
//---------------------------------------------------------------------------
inline constexpr float BGM_VOLUME            = 0.5f;
inline constexpr float SFX_GATE_VOLUME       = 0.8f;
inline constexpr float SFX_ZOMBIE_VOLUME     = 0.6f;
inline constexpr float SFX_HELI_VOLUME       = 0.7f;
inline constexpr float SFX_HURT_VOLUME       = 0.8f;
inline constexpr float ZOMBIE_GROAN_COOLDOWN = 3.0f;    //min seconds between 2 groans, 20 zombies would be noise(うめき声の最短間隔・秒。20体だと騒音になる)

}    // namespace SniperVsZombie::Config
