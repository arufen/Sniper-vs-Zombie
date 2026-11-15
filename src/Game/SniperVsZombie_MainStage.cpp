//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_MainStage.cpp
//! @brief	SniperVsZombie_MainStage
//---------------------------------------------------------------------------
#include "SniperVsZombie_MainStage.h"
#include "ObjectNames.h"
#include "StageConfig.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_Sky.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_Helicopter.h"
#include "SniperVsZombie_Camera.h"
#include "SniperVsZombie_Player.h"
#include "SniperVsZombie_Gate.h"
#include "SniperVsZombie_GameOver.h"
#include <Game/Component/ComponentBlackScreen.h>
#include <algorithm>

namespace SniperVsZombie {

//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_MainStage::Init()
{
    ResetStageState();

    //GATE
    auto gate = Scene::Object::Create<Gate>();
    gate->SetTranslate(Config::GATE_POSITION);
    gate->SetScaleAxisXYZ(Config::GATE_SCALE);
    gate->SetRotationAxisXYZ(Config::GATE_ROTATION);
    gateMaxHP_ = gate->GetMaxHP();
    gateHP_    = gate->GetHP();
    gatePos_   = Config::GATE_POSITION;    // save it so PostDraw can use it for the arrow marker

    //CAMERA
    auto camera = Scene::Object::Create<Camera>()->SetTranslate(Config::PLAYER_SPAWN_POSITION);
    camera->SetRotationAxisXYZ(Config::PLAYER_SPAWN_ROTATION);

    //PLAYER - hit body + HP. view and shooting stay in the Camera above(プレイヤー。当たり判定とHP。視点と射撃は上のCamera)
    auto player  = Scene::Object::Create<Player>()->SetTranslate(Config::PLAYER_SPAWN_POSITION + Config::PLAYER_EYE_TO_BODY);
    playerStats_ = player->GetComponent<ComponentPlayerStats>();

    //MAP
    CreateMap();

    //ZOMBIES - every death goes to OnZombieKilled(ゾンビ。全ての死亡はOnZombieKilledへ)
    spawner_.SetOnZombieKilled([this](const ComponentEnemyDeath::KillInfo& kill) { OnZombieKilled(kill); });

    //first one goes out right away, the rest come from the spawner timer in Update()(最初の1体はすぐ出す。残りはUpdateのスポナータイマーから)
    spawner_.SpawnZombie(ZombieType::Normal);

    //TEST: stalker right away. remove this line when the stalker is tuned(テスト: すぐストーカーを出す。調整が終わったらこの行を消す)
    spawner_.SpawnZombie(ZombieType::Stalker);

    //UI
    {
        hitMarkerHandle_    = LoadGraph("data/Game/UI/hitmarker2.png");
        hitMarkerRedHandle_ = LoadGraph("data/Game/UI/hitmarker2_headshot.png");    // red version for headshots
        arrowHandle_        = LoadGraph("data/Game/UI/arrow.png");                  // gate direction marker
    }

    //EFFECT
    hitEffectHandle_ = LoadEffekseerEffect("data/Sample/Effects/hit_eff.efkefc");

    //BLACK SCREEN TRANSITION
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName(Names::BLACK_SCREEN);
    blackScreen_ = blackScreenObj->AddComponent<ComponentBlackScreen>();
    blackScreen_->StartFadeIn();    // start fully black, fade INTO the stage

    return true;
}

void SniperVsZombie_MainStage::ResetStageState()
{
    spawner_.Reset();

    score_               = 0;
    isGameOver_          = false;
    isWin_               = false;
    civilianSaved_       = 0;
    civilianSaveTime_    = Config::CIVILIAN_SAVE_INTERVAL;
    hitmarkerAlpha_      = 0;
    hitmarkerIsHeadshot_ = false;
    arrowTimer_          = 0.0f;
}

void SniperVsZombie_MainStage::CreateMap()
{
    Scene::Object::Create<City>();

    auto woodTower = Scene::Object::Create<WoodTower>();
    woodTower->SetTranslate(Config::WOOD_TOWER_POSITION);
    woodTower->SetRotationAxisXYZ(Config::WOOD_TOWER_ROTATION);

    Scene::Object::Create<Sky>();

    auto helicopter = Scene::Object::Create<Helicopter>();
    helicopter->SetTranslate(Config::HELI_POSITION);
    helicopter->SetScaleAxisXYZ(Config::HELI_SCALE);
    helicopter->SetRotationAxisXYZ(Config::HELI_ROTATION_STAGE);
}

void SniperVsZombie_MainStage::OnZombieKilled(const ComponentEnemyDeath::KillInfo& kill)
{
    PlayHitEffect(kill.position);
    TriggerHitmark(kill.headshot);
    AddScore(kill.score);
}

void SniperVsZombie_MainStage::PlayHitEffect(const float3& position)
{
    int play_handle = PlayEffekseer3DEffect(hitEffectHandle_);
    SetPosPlayingEffekseer3DEffect(play_handle, position.x, position.y + 10.0f, position.z);
    SetScalePlayingEffekseer3DEffect(play_handle, 5.0f, 5.0f, 5.0f);
}

void SniperVsZombie_MainStage::TriggerHitmark(bool isHeadshot)
{
    hitmarkerAlpha_      = 255;
    hitmarkerIsHeadshot_ = isHeadshot;
}

void SniperVsZombie_MainStage::AddScore(int num)
{
    score_ += num;
}

void SniperVsZombie_MainStage::UpdateCivilian()
{
    civilianSaveTime_ -= GetDeltaTime();
    if(civilianSaveTime_ < 0.0f) {
        civilianSaveTime_ = Config::CIVILIAN_SAVE_INTERVAL;

        //1 civilian saved every CIVILIAN_SAVE_INTERVAL seconds(CIVILIAN_SAVE_INTERVAL秒ごとに1人救出)
        if(civilianSaved_ < Config::CIVILIAN_TOTAL)
            civilianSaved_++;
    }
}

float SniperVsZombie_MainStage::GetStageProgress() const
{
    return std::clamp(static_cast<float>(civilianSaved_) / static_cast<float>(Config::CIVILIAN_TOTAL), 0.0f, 1.0f);
}

void SniperVsZombie_MainStage::OnGameOver(bool isWin, bool breakGate)
{
    isGameOver_ = true;
    isWin_      = isWin;

    //play the broken gate only when zombies broke it, not when the player died(ゲートが壊された時だけ壊れ演出。プレイヤー死亡では出さない)
    if(breakGate) {
        if(auto gate = Scene::Object::Get<Gate>(Names::GATE)) {
            gate->DestroyGate();
        }
    }

    // capture isWin + the score AS IT IS RIGHT NOW into the lambda, so the callback
    // still has the correct values later even if score_ changes in the meantime
    blackScreen_->StartFadeOut([isWin, finalScore = score_]() {
        auto gameOverScene = Scene::GetScene<SniperVsZombie_GameOver>();
        gameOverScene->SetResult(isWin, finalScore);
        Scene::Change(gameOverScene);
    });
}

void SniperVsZombie_MainStage::Update()
{
    //ZOMBIES - no new zombies once the game is over(ゾンビ。ゲーム終了後は新しく出さない)
    if(!isGameOver_)
        spawner_.Update(GetDeltaTime(), GetStageProgress());

    //Update civilians count
    UpdateCivilian();

    //Get gate HP
    if(auto gate = Scene::Object::Get<Gate>(Names::GATE)) {
        gateHP_ = gate->GetHP();
    }

    //Check Gameover
    if(!isGameOver_) {
        bool isPlayerDead = false;
        if(auto stats = playerStats_.lock())
            isPlayerDead = stats->IsDead();

        if(gateHP_ <= 0)
            OnGameOver(false, true);    //lose, gate broke(負け。ゲートが壊れた)
        else if(isPlayerDead)
            OnGameOver(false, false);    //lose, player died(負け。プレイヤーが死んだ)
        else if(civilianSaved_ >= Config::CIVILIAN_TOTAL)
            OnGameOver(true, false);    //win(勝ち)
    }

    //Hit marker UI
    if(hitmarkerAlpha_ > 0)
        hitmarkerAlpha_ -= 3;

    //GATE ARROW INDICATOR - tick the bounce timer every frame
    arrowTimer_ += GetDeltaTime();
}

void SniperVsZombie_MainStage::Exit()
{
    DeleteEffekseerEffect(hitEffectHandle_);
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

            float rate  = static_cast<float>(civilianSaved_) / static_cast<float>(Config::CIVILIAN_TOTAL);
            int   color = GetColor(static_cast<int>(255.0f * (1.0f - rate)), static_cast<int>(255.0f * rate), 0);

            DrawFormatString(x, y, color, message, civilianSaved_, Config::CIVILIAN_TOTAL);
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

    //GATE ARROW INDICATOR - bouncing green arrow pointing down at the gate
    {
        // lift the marker above the gate model so it's not stuck inside it
        float3 markerWorldPos = gatePos_ + float3{0.0f, 50.0f, 0.0f};

        // sin bounce: goes up/down smoothly forever. 3.0f = speed, 10.0f = how far it moves
        float bounceOffset = sinf(arrowTimer_ * 3.0f) * 10.0f;

        VECTOR screenPos = ConvWorldPosToScreenPos(VGet(markerWorldPos.x, markerWorldPos.y, markerWorldPos.z));

        // only draw if the gate is actually in front of the camera (0~1 range = visible depth)
        if(screenPos.z > 0.0f && screenPos.z < 1.0f) {
            float x     = screenPos.x;
            float y     = screenPos.y + bounceOffset;
            float scale = 0.1f;    // adjust till the arrow size looks right

            DrawRotaGraphF(x, y, scale, 0.0f, arrowHandle_, TRUE);
        }
    }

    //PLAYER HP UI + red hit flash(プレイヤーHP表示と被弾の赤フラッシュ)
    if(auto stats = playerStats_.lock())
        stats->DrawHUD();

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades
    blackScreen_->DrawBlackScreen();

    //GAME OVER
    if(isGameOver_) {
        SetFontSize(64);
        if(isWin_) {
            DrawTextWithAlign("Rescue complete", GetColor(0, 255, 0), 0, WINDOW_H / 2 - 32, 1);
            SetFontSize(50);
            DrawTextWithAlign("救出作戦完了", GetColor(0, 255, 0), 0, WINDOW_H / 2 + 32, 1);
            SetFontSize(64);
        }
        else {
            DrawTextWithAlign("Rescue failed", GetColor(255, 0, 0), 0, WINDOW_H / 2 - 32, 1);
            SetFontSize(50);
            DrawTextWithAlign("救出作戦完了", GetColor(255, 0, 0), 0, WINDOW_H / 2 + 32, 1);
            SetFontSize(64);
        }
    }

    SetFontSize(20);
}

}    // namespace SniperVsZombie
