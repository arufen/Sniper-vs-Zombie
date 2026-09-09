//---------------------------------------------------------------------------
//!	@file	Tutorial_X.cpp
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include "SniperVsZombie_MainStage.h"
#include "SniperVsZombie_Player.h"
#include "SniperVsZombie_Enemy.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_Helicopter.h"
#include "SniperVsZombie_Camera.h"
#include "SniperVsZombie_Gate.h"
#include "Random.h"
#include <vector>

namespace SniperVsZombie {
static const float3 PLAYER_SPAWN_POSITION = float3{447.147f, 231.806f, 585.323f};

//Enemy Spawn points
static const std::vector<float3> enemySpawnPos = {
    { -62.530f, 28.521f, 309.090f},
    {-358.739f, 24.193f, 501.069f},
    { 460.635f, 18.468f, 707.177f},
};

//Waypoints each enemy walks through before reaching the gate.
//enemyPathFinding[i] is the path used by an enemy spawned at enemySpawnPos[i].
static const std::vector<std::vector<float3>> enemyPathFinding = {
    {{236.747f, 0.139f, 316.820f}, {233.324f, 0.139f, 671.936f}, {70.552f, 0.139f, 726.139f}},
    {{-332.641f, 0.139f, 685.004f}, {64.192f, 0.139f, 719.587f}},
    {{254.435f, 0.139f, 678.518}, {64.192f, 0.139f, 719.587f}},
};

void SniperVsZombie_MainStage::AddScore(int num)
{
    score_ += num;
}
//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_MainStage::Init()
{
    //GATE
    auto   gate      = Scene::Object::Create<Gate>();
    float3 gatePos   = {67.726, 0.911, 760.512};
    float3 gateScale = {0.75f, 0.75f, 0.75f};
    gate->SetTranslate(gatePos);
    gate->SetScaleAxisXYZ(gateScale);
    gate->SetRotationAxisXYZ({0.0f, 90.0f, 0.0f});
    gateMaxHP_ = gate->GetMaxHP();

    //CAMERA
    auto camera = Scene::Object::Create<Camera>()->SetTranslate(PLAYER_SPAWN_POSITION);
    camera->SetRotationAxisXYZ({0.0f, -180.0f, 0.0f});

    //ENEMY - first one goes out right away, rest come from the spawn timer in Update()
    CreateEnemy();

    //MAP
    {
        Scene::Object::Create<City>();
        auto   woodTower     = Scene::Object::Create<WoodTower>();
        float3 watchTowerPos = {437.0f, 100.0f, 550.0f};
        woodTower->SetTranslate({watchTowerPos});
        woodTower->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});

        auto   helicopter = Scene::Object::Create<Helicopter>();
        float3 heliPos    = {1.360f, 414.269f, 700.566f};
        float3 heliScale  = {0.75f, 0.75f, 0.75f};
        float3 heliRot    = {0.0f, -153.0f, 0.0f};
        helicopter->SetTranslate(heliPos);
        helicopter->SetScaleAxisXYZ(heliScale);
        helicopter->SetRotationAxisXYZ(heliRot);
    }

    //UI
    {
        hitMarkerHandle_    = LoadGraph("data/Game/UI/hitmarker2.png");
        hitMarkerRedHandle_ = LoadGraph("data/Game/UI/hitmarker2_headshot.png");    // red version for headshots
        hitmarkerAlpha_     = 0;
    }

    return true;
}

void SniperVsZombie_MainStage::CreateEnemy()
{
    // hard cap on total spawns so we don't flood ram with enemies over a long game
    if(totalEnemiesSpawned_ >= MAX_ENEMIES)
        return;

    int spawnIndex = rand() % static_cast<int>(enemySpawnPos.size());

    auto newEnemy = Scene::Object::Create<Enemy>();
    newEnemy->SetTranslate(enemySpawnPos[spawnIndex]);
    newEnemy->SetPath(enemyPathFinding[spawnIndex]);

    totalEnemiesSpawned_++;
}

void SniperVsZombie_MainStage::UpdateSpawnTimer()
{
    time_to_spawn_enemy -= GetDeltaTime();
}

void SniperVsZombie_MainStage::SetIntervalSpawnTime(float newTimer)
{
    time_to_spawn_enemy_interval = newTimer;
}

void SniperVsZombie_MainStage::UpdateCivilian()
{
    if(civilianSaveTime_ >= 0.0f) {
        civilianSaveTime_ -= GetDeltaTime();
    }
    else {
        civilianSaveTime_ = civilianSaveTimeInterval_;

        //Saved civilian every 5s
        if(civilianSaved_ < 100)
            civilianSaved_++;
    }
}

void SniperVsZombie_MainStage::StageDifficultyUpdate()
{
    // how far the stage is (0.0 = just started, 1.0 = all civilians saved)
    float rate = static_cast<float>(civilianSaved_) / static_cast<float>(civilianTotalCount_);
    rate       = std::clamp(rate, 0.0f, 1.0f);

    // interval goes from 5.0s (chill start) down to 0.5s (crazy end)
    float newInterval = 5.0f - (5.0f - 0.5f) * rate;

    SetIntervalSpawnTime(newInterval);
}

void SniperVsZombie_MainStage::TriggerHitmark(bool isHeadshot)
{
    hitmarkerAlpha_      = 255;
    hitmarkerIsHeadshot_ = isHeadshot;
}

void SniperVsZombie_MainStage::OnGameOver()
{
    isGameOver_ = true;

    if(auto gate = Scene::Object::Get<Gate>("Gate")) {
        gate->DestroyGate();
    }
}

void SniperVsZombie_MainStage::Update()
{
    //ENEMY SPAWN - keeps going until totalEnemiesSpawned_ hits MAX_ENEMIES
    if(totalEnemiesSpawned_ < MAX_ENEMIES) {
        if(time_to_spawn_enemy <= 0.0f) {
            time_to_spawn_enemy = time_to_spawn_enemy_interval;
            CreateEnemy();
        }
        else {
            UpdateSpawnTimer();
        }
    }

    UpdateCivilian();

    if(auto gate = Scene::Object::Get<Gate>("Gate")) {
        gateHP_ = gate->GetHP();
    }

    if(gateHP_ <= 0) {
        if(!isGameOver_) {
            OnGameOver();
        }
    }

    //Stage Difficulty adjust
    StageDifficultyUpdate();

    //Hit marker UI
    {
        if(hitmarkerAlpha_ > 0)
            hitmarkerAlpha_ -= 3;
    }
}

//align 0 = left, align 1 = middle, align 2 = right
void DrawTextWithAlign(const char* message, int color, int offsetX = 0, int offsetY = 0, int align = 0)
{
    // 書式付き文字列の描画幅・高さ・行数を取得する
    s32 width;         // 幅
    s32 height;        // 高さ
    s32 line_count;    // 行数
    GetDrawFormatStringSize(&width, &height, &line_count, message);

    //Position
    s32 x;
    if(align == 0)
        x = 0;
    if(align == 1)
        x = WINDOW_W / 2 - (width / 2);
    if(align == 2)
        x = (WINDOW_W - width);

    s32 y = 32;
    DrawFormatString(x + offsetX, y + offsetY, color, message);
}

void SniperVsZombie_MainStage::PostDraw()
{
    __super::PostDraw();

    //SCORE
    DrawFormatString(10, 50, GetColor(255, 255, 255), "スコア: %d", score_);

    //HP UI
    {
        float HP           = gateHP_;
        float maxHP        = gateMaxHP_;
        float x1           = 10.0f;
        float y1           = 100.0f;
        float x2           = 200.0f;
        float y2           = 130.0f;
        int   outlineColor = GetColor(0, 0, 0);
        //Outline
        DrawBoxAA(x1, y1, x2, y2, outlineColor, 1);
        //inward offset
        float offsetX = 5.0f;
        float offsetY = 5.0f;
        //Main color
        int mainColor = GetColor(0, 255, 0);

        //  calculate HP ratio (0.0 ~ 1.0)
        float hpRatio = std::clamp(HP / maxHP, 0.0f, 1.0f);

        //  inner bar's max width (based on outline minus offsets)
        float innerMaxWidth = (x2 - offsetX) - (x1 + offsetX);

        //  scale width according to current HP
        float innerX2 = (x1 + offsetX) + innerMaxWidth * hpRatio;

        //Text Hp
        DrawFormatString(10, 70, GetColor(255, 255, 255), "ドア HP: %d", static_cast<int>(gateHP_));

        //Main HP
        DrawBoxAA(x1 + offsetX, y1 + offsetY, innerX2, y2 - offsetY, mainColor, 1);
    }

    //Tutorial UI
    {
        int tutorialColor = GetColor(255, 255, 255);
        {
            // 書式付き文字列の描画幅・高さ・行数を取得する
            const char* message = "右クリックでカメラを動かす";
            s32         width;         // 幅
            s32         height;        // 高さ
            s32         line_count;    // 行数
            GetDrawFormatStringSize(&width, &height, &line_count, message);

            // センタリング
            s32 x = (WINDOW_W - width);
            s32 y = height + 32;

            DrawFormatString(x, y, tutorialColor, message);
        }

        {
            // 書式付き文字列の描画幅・高さ・行数を取得する
            const char* message = "左クリックで弾を撃つ";
            s32         width;         // 幅
            s32         height;        // 高さ
            s32         line_count;    // 行数
            GetDrawFormatStringSize(&width, &height, &line_count, message);

            // センタリング
            s32 x = (WINDOW_W - width);
            s32 y = height * 2 + 32;

            DrawFormatString(x, y, tutorialColor, message);
        }

        {
            // 書式付き文字列の描画幅・高さ・行数を取得する
            const char* message = "Eで覗き込み";
            s32         width;         // 幅
            s32         height;        // 高さ
            s32         line_count;    // 行数
            GetDrawFormatStringSize(&width, &height, &line_count, message);

            // センタリング
            s32 x = (WINDOW_W - width);
            s32 y = height * 3 + 32;

            DrawFormatString(x, y, tutorialColor, message);
        }

        {
            // 書式付き文字列の描画幅・高さ・行数を取得する
            const char* message = "避難者: %d / %d";
            s32         width;         // 幅
            s32         height;        // 高さ
            s32         line_count;    // 行数
            GetDrawFormatStringSize(&width, &height, &line_count, message);

            // センタリング
            s32 x = 10;
            s32 y = height * 5 + 32;

            DrawFormatString(x, y, tutorialColor, message, civilianSaved_, civilianTotalCount_);
        }
    }

    //Hitmarker UI
    {
        float x           = WINDOW_W / 2;
        float y           = WINDOW_H / 2;
        int   handleToUse = hitmarkerIsHeadshot_ ? hitMarkerRedHandle_ : hitMarkerHandle_;

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, hitmarkerAlpha_);    // Blend Everything Below the syntax (NOBLEND is essential)
        DrawRotaGraphF(x, y, 0.5, 0.0f, handleToUse, 1);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);    //stop the blend  (0 means nothing)
    }

    //GAME OVER
    if(isGameOver_) {
        //DrawFormatString(400, 300, GetColor(255, 0, 0), " Rescue aborted 救出作戦中止");
        SetFontSize(64);
        DrawTextWithAlign("Rescue aborted 救出作戦中止", GetColor(255, 0, 0), 0, WINDOW_H / 2 - 32, 1);
    }

    SetFontSize(20);

    clsDx();
    printfDx("interval: %f", time_to_spawn_enemy_interval);
}

}    // namespace SniperVsZombie
