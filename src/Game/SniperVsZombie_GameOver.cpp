//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_GameOver.cpp
//! @brief	SniperVsZombie_GameOver
//---------------------------------------------------------------------------
#include "SniperVsZombie_GameOver.h"
#include "ObjectNames.h"
#include "ShaderCamera.h"
#include "CameraProbe.h"
#include "StageConfig.h"
#include "StoryText.h"
#include "AssetPaths.h"
#include "SoundUtil.h"
#include "HudText.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_Sky.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_Helicopter.h"
#include "SniperVsZombie_NormalZombie.h"
#include "SniperVsZombie_MainStage.h"
#include "SniperVsZombie_MainMenu.h"
#include "SniperVsZombie_Gate.h"
#include <Game/Component/ComponentBlackScreen.h>
#include <Game/Component/ComponentButton.h>
#include <Game/Component/ComponentFlightPath.h>
#include <algorithm>

namespace SniperVsZombie {

namespace {
constexpr int SNIPER_DOWN_TINT_ALPHA = 60;    //light red over the whole sniper-down scene(狙撃手ダウンの場面全体に薄い赤)
}

//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_GameOver::Init()
{
    //the scene object is reused between plays, so reset everything(シーンは使い回されるので全部リセット)
    resultDelay_      = -1.0f;
    showResult_       = false;
    winRadioStep_     = 0;
    wasLeftMouseDown_ = true;
    retryButton_.reset();
    menuButton_.reset();

    //MAP - same positions as MainStage, from StageConfig.h(マップ。MainStageと同じ位置をStageConfig.hから)
    Scene::Object::Create<City>();
    auto woodTower = Scene::Object::Create<WoodTower>();
    woodTower->SetTranslate(Config::WOOD_TOWER_POSITION);
    woodTower->SetRotationAxisXYZ(Config::WOOD_TOWER_ROTATION);

    //Sky
    Scene::Object::Create<Sky>();
    Scene::Object::Create<ShaderCamera>();    //sends the real camera position to ps_model.fx(本当のカメラ位置をps_model.fxへ送る)
    CreateCameraProbe();                      //dev tool: F7 show, F8 copy camera position. Debug only(開発ツール: F7表示、F8でカメラ位置をコピー。Debugのみ)

    //Load helicopter - parked on the roof unless the win cutscene flies it(ヘリ。勝利カットシーン以外は屋上に停まったまま)
    auto heli = Scene::Object::Create<Helicopter>();
    heli->SetTranslate(Config::HELI_POSITION);
    heli->SetScaleAxisXYZ(Config::HELI_SCALE);
    heli->SetRotationAxisXYZ(Config::HELI_ROTATION_STAGE);
    helicopter_ = heli;

    //GATE - a different name, so the cutscene zombie walks to the fake "Gate" point instead(別名にする。カットシーンのゾンビは偽"Gate"地点へ歩く)
    auto gate = Scene::Object::Create<Gate>();
    gate->SetName(Names::GATE_CUTSCENE);
    gate->RemoveComponent<ComponentCollisionModel>();    // don't need collision for the cutscene
    gate->SetTranslate(Config::GATE_POSITION);
    gate->SetScaleAxisXYZ(Config::GATE_SCALE);
    gate->SetRotationAxisXYZ(Config::GATE_ROTATION);

    //RADIO(無線)
    radio_ = CreateRadio();

    //CUTSCENE CAMERA - fades in by itself, so the 2s black screen fade does not hide the start(自分でフェードインする。2秒の暗転で冒頭が隠れないように)
    auto camera = Scene::Object::Create<Object>();
    camera->SetName(Names::CAMERA);
    cutscene_ = camera->AddComponent<ComponentCameraPath>();
    cutscene_->SetFadeInAtStart(true)->SetOnFinished([this]() { OnCutsceneFinished(); });

    //the result (set by MainStage) picks the ending(MainStageが設定した結果でエンディングを選ぶ)
    switch(result_) {
    case GameResult::Win:
        SetupWin();
        break;
    case GameResult::GateBreached:
        SetupGateBreached();
        break;
    case GameResult::SniperDown:
        SetupSniperDown();
        break;
    }
    cutscene_->Play();

    //BLACK SCREEN TRANSITION - only for leaving(退出用のみ)
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName(Names::BLACK_SCREEN);
    blackScreen_ = blackScreenObj->AddComponent<ComponentBlackScreen>();

    return true;
}

void SniperVsZombie_GameOver::SetupWin()
{
    //HELICOPTER: W1 -> W2 (arrive) -> hover = pickup -> W3 (leave)(ヘリ: W1→W2(到着)→ホバリング=乗り込み→W3(離脱))
    if(auto heli = helicopter_.lock()) {
        heli->AddComponent<ComponentFlightPath>()
            ->AddPoint(Config::HELI_WIN_START, 0.0f)
            ->AddPoint(Config::HELI_WIN_HOVER, Config::HELI_FLY_IN_TIME)
            ->AddPoint(Config::HELI_WIN_HOVER, Config::HELI_HOVER_TIME)
            ->AddPoint(Config::HELI_WIN_LEAVE, Config::HELI_FLY_OUT_TIME)
            ->SetFaceTravel(true, Config::HELI_MODEL_YAW_OFFSET)
            ->SetBob(Config::HELI_HOVER_BOB)
            ->Play();
    }

    //CAMERA: one long smooth move C1 -> C2, always looking at the helicopter(カメラ: C1→C2の長い1ショット。常にヘリを見る)
    const float total = Config::HELI_FLY_IN_TIME + Config::HELI_HOVER_TIME + Config::HELI_FLY_OUT_TIME;
    cutscene_->SetShots({
        {Config::WIN_CAMERA_EYE_START, Config::HELI_WIN_START, Config::WIN_CAMERA_EYE_END, Config::HELI_WIN_LEAVE, total}
    });
    cutscene_->SetFollowTarget(helicopter_);

    radio_->Say(Story::RADIO_HELI_INBOUND);
    winRadioStep_ = 1;
}

void SniperVsZombie_GameOver::SetupGateBreached()
{
    //CAMERA: L10 -> L11, down to the broken gate(カメラ: L10→L11、壊れたゲートへ)
    cutscene_->SetShots({Config::GATE_BREACH_SHOT});

    //fake "Gate" point behind the gate, the zombie walks through toward it(ゲートの奥の偽"Gate"地点。ゾンビはそこへ向かって通り抜ける)
    auto path = Scene::Object::Create<Object>();
    path->SetTranslate({70.222f, 4.138f, 900.0f});
    path->SetName(Names::GATE);

    //the zombie that broke in(侵入したゾンビ)
    auto enemy = Scene::Object::Create<NormalZombie>();
    enemy->SetTranslate({70.260f, 3.801f, 740.620f});
    enemy->SetScaleAxisXYZ({0.7f, 0.7f, 0.7f});
    enemy->SetRotationAxisXYZ({0.0f, 180.0f, 0.0f});
    enemy->SetMoveSpeed(0.1f);    // now reliably applied on the enemy's first Update() tick

    if(auto gate = Scene::Object::Get<Gate>(Names::GATE_CUTSCENE))
        gate->DestroyGate();

    radio_->Say(Story::RADIO_GATE_LOST);
}

void SniperVsZombie_GameOver::SetupSniperDown()
{
    //CAMERA: S10 -> S11 on the tower(カメラ: 塔の上でS10→S11)
    cutscene_->SetShots({Config::SNIPER_DOWN_SHOT});

    //D2 = where the stalker's jump ends (both lanes end on the tower). D1 = the player spot, same floor height(D2=ストーカーのジャンプの終点。D1=プレイヤーの位置、同じ床の高さ)
    const float3 stalker_pos = Config::STALKER_LANES.front().jumps.back();
    const float3 body_pos    = {Config::PLAYER_SPAWN_POSITION.x, stalker_pos.y, Config::PLAYER_SPAWN_POSITION.z};

    //DEAD SNIPER - only if the new player model exists (see RequiredAssets.md)(倒れた狙撃手。新しいプレイヤーモデルがある時だけ)
    if(Sound::FileExists(Assets::PLAYER_MODEL)) {
        auto body = Scene::Object::Create<Object>();
        body->SetName(Names::CUTSCENE_PLAYER);
        body->SetTranslate(body_pos);
        body->SetRotationToPosition(stalker_pos);    //fall facing the stalker(ストーカーの方を向いて倒れる)

        auto model = body->AddComponent<ComponentModel>(Assets::PLAYER_MODEL);
        if(Sound::FileExists(Assets::PLAYER_ANIM_DEATH)) {
            model->SetAnimation({
                {"death", Assets::PLAYER_ANIM_DEATH, 0, 1.0f}
            });
            model->PlayAnimation("death", false);
        }
        else {
            //no death anim yet: lie the model down by code(死亡アニメがまだ無い: コードでモデルを寝かせる)
            float3 rot = body->GetRotationAxisXYZ();
            body->SetRotationAxisXYZ({-90.0f, rot.y, rot.z});
        }
    }

    //STALKER - model only (no AI, no collision), attacking the body forever(ストーカー。モデルだけ(AI・当たり判定なし)、体を攻撃し続ける)
    auto stalker = Scene::Object::Create<Object>();
    stalker->SetName(Names::CUTSCENE_STALKER);
    stalker->SetTranslate(stalker_pos);
    stalker->SetRotationToPosition(body_pos);
    auto model = stalker->AddComponent<ComponentModel>(Assets::ZOMBIE_MODEL);
    model->SetAnimation({
        {"attack", Assets::ZOMBIE_ANIM_ATTACK, 0, 1.0f}
    });
    model->PlayAnimation("attack", true);

    radio_->Say(Story::RADIO_SNIPER_LOST);
}

void SniperVsZombie_GameOver::UpdateWinRadio()
{
    const float t = cutscene_ ? cutscene_->GetTime() : 0.0f;

    //heli arrives at W2 -> "get in", heli leaves -> "extraction complete"(ヘリがW2に着く→「乗れ」、離脱→「脱出成功」)
    if(winRadioStep_ == 1 && t >= Config::HELI_FLY_IN_TIME) {
        radio_->Say(Story::RADIO_HELI_PICKUP);
        winRadioStep_ = 2;
    }
    else if(winRadioStep_ == 2 && t >= Config::HELI_FLY_IN_TIME + Config::HELI_HOVER_TIME) {
        radio_->Say(Story::RADIO_HELI_ESCAPE);
        winRadioStep_ = 3;
    }
}

void SniperVsZombie_GameOver::OnCutsceneFinished()
{
    resultDelay_ = Config::RESULT_DELAY;
}

void SniperVsZombie_GameOver::ShowResult()
{
    showResult_ = true;

    //BUTTONS - made only now, so they can not be clicked during the cutscene(ボタン。今作るのでカットシーン中は押せない)
    auto retryButtonObj = Scene::Object::Create<Object>();
    retryButtonObj->SetName("RetryButton");
    retryButton_ = retryButtonObj->AddComponent<ComponentButton>();
    retryButton_->Setup({66.0f, 400.0f}, {200.0f, 60.0f}, "RETRY", GetColor(0, 200, 0), [this]() {
        blackScreen_->StartFadeOut([]() { Scene::Change(Scene::GetScene<SniperVsZombie_MainStage>()); });
    });

    auto menuButtonObj = Scene::Object::Create<Object>();
    menuButtonObj->SetName("MenuButton");
    menuButton_ = menuButtonObj->AddComponent<ComponentButton>();
    menuButton_->Setup({66.0f, 480.0f}, {200.0f, 60.0f}, "MENU", GetColor(200, 0, 0), [this]() {
        blackScreen_->StartFadeOut([]() { Scene::Change(Scene::GetScene<SniperVsZombie_MainMenu>()); });
    });
}

void SniperVsZombie_GameOver::Update()
{
    __super::Update();

    //SKIP - click during the cutscene jumps to the result(スキップ。カットシーン中のクリックで結果へ)
    const bool left_down   = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
    const bool click_start = left_down && !wasLeftMouseDown_;
    wasLeftMouseDown_      = left_down;

    if(cutscene_ && cutscene_->IsPlaying()) {
        if(isWin_)
            UpdateWinRadio();
        if(click_start) {
            cutscene_->Skip();
            radio_->Clear();
            resultDelay_ = 0.0f;    //no extra wait after a skip(スキップ後は待たない)
        }
    }

    //RESULT - after the short delay(結果。短い待ちの後)
    if(resultDelay_ >= 0.0f && !showResult_) {
        resultDelay_ -= GetDeltaTime();
        if(resultDelay_ <= 0.0f)
            ShowResult();
    }
}

void SniperVsZombie_GameOver::PostDraw()
{
    __super::PostDraw();

    //SNIPER DOWN - light red tint over the whole scene(狙撃手ダウン。場面全体に薄い赤)
    if(result_ == GameResult::SniperDown) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, SNIPER_DOWN_TINT_ALPHA);
        DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(120, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    if(cutscene_)
        cutscene_->DrawFade();
    radio_->DrawRadio();

    //RESULT TEXT + BUTTONS - only after the cutscene(結果の文字とボタン。カットシーンの後だけ)
    if(showResult_) {
        SetFontSize(64);
        if(isWin_) {
            DrawTextWithAlign(Story::RESULT_WIN_EN, GetColor(0, 255, 0), 0, WINDOW_H / 2 - 32, 1);
            SetFontSize(50);
            DrawTextWithAlign(Story::RESULT_WIN_JP, GetColor(0, 255, 0), 0, WINDOW_H / 2 + 32, 1);
        }
        else {
            DrawTextWithAlign(Story::RESULT_LOSE_EN, GetColor(255, 0, 0), 0, WINDOW_H / 2 - 32, 1);
            SetFontSize(50);
            //was "救出作戦完了" (complete) for a loss too, a copy-paste bug(負けでも「完了」になっていた。コピペのバグ)
            DrawTextWithAlign(
                result_ == GameResult::SniperDown ? Story::RESULT_SNIPER_JP : Story::RESULT_GATE_JP, GetColor(255, 0, 0), 0, WINDOW_H / 2 + 32, 1);
        }

        SetFontSize(30);
        DrawFormatString(WINDOW_W / 2 - 100, WINDOW_H / 2 + 130, GetColor(255, 255, 255), Story::RESULT_SCORE, finalScore_);
        SetFontSize(20);

        if(retryButton_)
            retryButton_->DrawButton();
        if(menuButton_)
            menuButton_->DrawButton();
    }

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades
    blackScreen_->DrawBlackScreen();
}

void SniperVsZombie_GameOver::Exit()
{
    Hud::ReleaseFont();    //the radio used the HUD font(無線がHUDフォントを使った)
}

}    // namespace SniperVsZombie
