//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_MainStage.cpp
//! @brief	SniperVsZombie_MainStage
//---------------------------------------------------------------------------
#include "SniperVsZombie_MainStage.h"
#include "ObjectNames.h"
#include "ShaderCamera.h"
#include "CameraProbe.h"
#include "StageConfig.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_Sky.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_Helicopter.h"
#include "SniperVsZombie_Camera.h"
#include "SniperVsZombie_Player.h"
#include "SniperVsZombie_Gate.h"
#include "SniperVsZombie_GameOver.h"
#include "HudText.h"
#include <Game/Component/ComponentFPSCameraController.h>
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
    Scene::Object::Create<ShaderCamera>();    //sends the real camera position to ps_model.fx(本当のカメラ位置をps_model.fxへ送る)
    CreateCameraProbe();                      //dev tool: F7 show, F8 copy camera position. Debug only(開発ツール: F7表示、F8でカメラ位置をコピー。Debugのみ)

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
    Hud::ReleaseFont();    //free the HUD font, it is made again next time it is needed(HUDフォントを解放。次に必要な時に作り直される)
}

void SniperVsZombie_MainStage::PostDraw()
{
    __super::PostDraw();

    //----------------------------------------------------------
    // HUD - all text uses Hud:: = bigger font with a black outline(HUD - 文字は全部Hud::を使う=大きめで黒い縁取り付き)
    //----------------------------------------------------------
    const int          line  = Hud::GetLineHeight();
    const unsigned int white = GetColor(255, 255, 255);

    //SCORE(スコア)
    const int left_y = 50;
    Hud::DrawLabel(10, left_y, white, "スコア: %d", score_);

    //GATE HP: text, then the bar under it(ゲートHP: 文字、その下にバー)
    {
        Hud::DrawLabel(10, left_y + line, white, "ドア HP: %d", static_cast<int>(gateHP_));

        float x1      = 10.0f;
        float y1      = static_cast<float>(left_y + line * 2 + 4);
        float x2      = 270.0f;    //was 200, a little bigger now(以前は200。少し大きく)
        float y2      = y1 + 34.0f;
        float offset  = 5.0f;    //inward offset(内側の余白)
        float hpRatio = std::clamp(gateHP_ / gateMaxHP_, 0.0f, 1.0f);

        DrawBoxAA(x1, y1, x2, y2, GetColor(0, 0, 0), 1);    //outline(枠)

        float innerMaxWidth = (x2 - offset) - (x1 + offset);
        float innerX2       = (x1 + offset) + innerMaxWidth * hpRatio;
        DrawBoxAA(x1 + offset, y1 + offset, innerX2, y2 - offset, GetColor(0, 255, 0), 1);
    }

    //CIVILIANS: color goes red -> green as more are saved(避難者: 救出が増えると赤から緑へ)
    {
        float rate  = static_cast<float>(civilianSaved_) / static_cast<float>(Config::CIVILIAN_TOTAL);
        int   color = GetColor(static_cast<int>(255.0f * (1.0f - rate)), static_cast<int>(255.0f * rate), 0);
        Hud::DrawLabel(10, left_y + line * 3 + 48, color, "避難者: %d / %d", civilianSaved_, Config::CIVILIAN_TOTAL);
    }

    //TUTORIAL (top right). the camera line changes when the mouse is locked(操作説明(右上)。マウスロック中はカメラの行が変わる)
    {
        bool locked = false;
        if(auto camera = Scene::Object::Get<Camera>())
            if(auto controller = camera->GetComponent<ComponentFPSCameraController>())
                locked = controller->IsMouseLocked();

        const int right_x = WINDOW_W - 10;
        const int top_y   = 50;

        Hud::DrawLabelRight(right_x, top_y, white, locked ? "マウスでカメラを動かす" : "右クリックでカメラを動かす");
        Hud::DrawLabelRight(right_x, top_y + line, white, "左クリックで弾を撃つ");
        Hud::DrawLabelRight(right_x, top_y + line * 2, white, "Eで覗き込み");

        //green when ON, so the player sees the state at a glance(ONの時は緑。状態がひと目で分かるように)
        const unsigned int lock_color = locked ? GetColor(80, 255, 80) : white;
        Hud::DrawLabelRight(right_x, top_y + line * 3, lock_color, "Qでマウスロック: %s", locked ? "ON" : "OFF");
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
