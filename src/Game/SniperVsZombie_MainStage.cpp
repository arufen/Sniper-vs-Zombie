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
#include "ThreatIndicator.h"
#include "SniperVsZombie_MainMenu.h"
#include "StoryText.h"
#include "SoundUtil.h"
#include "AssetPaths.h"
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
    gate->SetOnDamaged([this](float hp, float max_hp) { OnGateDamaged(hp, max_hp); });

    //CAMERA - the player's FPS view. frozen until the opening ends(プレイヤーのFPS視点。オープニングが終わるまで止めておく)
    auto camera = Scene::Object::Create<Camera>()->SetTranslate(Config::PLAYER_SPAWN_POSITION);
    camera->SetRotationAxisXYZ(Config::PLAYER_SPAWN_ROTATION);
    playerCamera_ = camera;
    SetPlayerControl(false);

    //PLAYER - hit body + HP. view and shooting stay in the Camera above(プレイヤー。当たり判定とHP。視点と射撃は上のCamera)
    auto player  = Scene::Object::Create<Player>()->SetTranslate(Config::PLAYER_SPAWN_POSITION + Config::PLAYER_EYE_TO_BODY);
    playerStats_ = player->GetComponent<ComponentPlayerStats>();

    //MAP
    CreateMap();

    //ZOMBIES - every death goes to OnZombieKilled. the first zombie comes when the opening ends(ゾンビ。全ての死亡はOnZombieKilledへ。最初の1体はオープニング後)
    spawner_.SetOnZombieKilled([this](const ComponentEnemyDeath::KillInfo& kill) { OnZombieKilled(kill); });
    spawner_.SetOnZombieSpawned([this](ZombieType type) { OnZombieSpawned(type); });

    //RADIO(無線)
    radio_ = CreateRadio();

    //UI
    {
        hitMarkerHandle_    = LoadGraph("data/Game/UI/hitmarker2.png");
        hitMarkerRedHandle_ = LoadGraph("data/Game/UI/hitmarker2_headshot.png");    // red version for headshots
        arrowHandle_        = LoadGraph("data/Game/UI/arrow.png");                  // gate direction marker
    }

    //EFFECT
    hitEffectHandle_ = LoadEffekseerEffect("data/Sample/Effects/hit_eff.efkefc");

    //SOUNDS - silent until the files exist (RequiredAssets.md)(ファイルができるまで無音)
    bgm_            = Sound::Load(Assets::BGM_STAGE);
    sfxGateHit_     = Sound::Load(Assets::SFX_GATE_HIT);
    sfxGateBreak_   = Sound::Load(Assets::SFX_GATE_BREAK);
    sfxZombieGroan_ = Sound::Load(Assets::SFX_ZOMBIE_GROAN);
    sfxStalker_     = Sound::Load(Assets::SFX_STALKER_SCREECH);
    sfxPlayerHurt_  = Sound::Load(Assets::SFX_PLAYER_HURT);
    sfxKill_        = Sound::Load(Assets::SFX_KILL_CONFIRM);
    sfxHeadshot_    = Sound::Load(Assets::SFX_HEADSHOT);
    Sound::PlayBgm(bgm_, Config::BGM_VOLUME);

    //BLACK SCREEN TRANSITION - only for leaving. the opening does its own short fade in(退出用のみ。オープニングが自分で短くフェードインする)
    auto blackScreenObj = Scene::Object::Create<Object>();
    blackScreenObj->SetName(Names::BLACK_SCREEN);
    blackScreen_ = blackScreenObj->AddComponent<ComponentBlackScreen>();

    //PAUSE MENU - hidden until P / Tab during play(ポーズメニュー。プレイ中にP/Tabを押すまで隠す)
    pauseMenu_.Create({
        [this]() { SetPaused(false); },
        [this]() { LeaveFromPause(true); },
        [this]() { LeaveFromPause(false); },
        [this](SettingsSliders::Kind kind, float value) { OnSettingChanged(kind, value); },
    });

    StartOpening();

    return true;
}

void SniperVsZombie_MainStage::StartOpening()
{
    stageState_ = StageState::Opening;

    auto cutscene_obj = Scene::Object::Create<Object>();
    cutscene_obj->SetName(Names::CUTSCENE_CAMERA);
    opening_ = cutscene_obj->AddComponent<ComponentCameraPath>();
    opening_->SetShots(Config::OPENING_SHOTS)
        ->SetFadeInAtStart(true)    //the 2s black screen fade would eat most of O1, so a short own fade(2秒の暗転だとO1がほぼ隠れるので、短い自前フェード)
        ->SetOnFinished([this]() { OnOpeningFinished(); });
    opening_->Play();
}

void SniperVsZombie_MainStage::OnOpeningFinished()
{
    //give the view back to the FPS camera. O3 ends exactly at P0, so it does not jump(FPSカメラに視点を返す。O3はP0で終わるので飛ばない)
    opening_->ReleaseCamera();

    if(auto camera = playerCamera_.lock()) {
        if(auto controller = camera->GetComponent<ComponentFPSCameraController>())
            controller->SetLookAngles(Config::PLAYER_START_YAW, Config::PLAYER_START_PITCH);
    }
    SetPlayerControl(true);

    stageState_ = StageState::Playing;

    //first zombie right away, the rest come from the spawner timer(最初の1体はすぐ。残りはスポナーのタイマーから)
    spawner_.SpawnZombie(ZombieType::Normal);

    radio_->Say(Story::RADIO_MISSION_START);
    radio_->Say(Story::RADIO_OBJECTIVE);
}

void SniperVsZombie_MainStage::SetPlayerControl(bool enable)
{
    auto camera = playerCamera_.lock();
    if(!camera)
        return;

    //NoUpdate stops look + shooting, NoDraw hides the gun and the scope(NoUpdateで視点と射撃を止め、NoDrawで銃とスコープを隠す)
    camera->SetStatus(Object::StatusBit::NoUpdate, !enable);
    camera->SetStatus(Object::StatusBit::NoDraw, !enable);
}

bool SniperVsZombie_MainStage::IsClickStarted()
{
    const bool left_down   = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
    const bool click_start = left_down && !wasLeftMouseDown_;
    wasLeftMouseDown_      = left_down;
    return click_start;
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
    popups_.Clear();
    kills_             = 0;
    headshots_         = 0;
    playTime_          = 0.0f;
    paused_            = false;
    leaving_           = false;
    arrowTimer_        = 0.0f;
    stageState_        = StageState::Opening;
    wasLeftMouseDown_  = true;
    said_              = SaidFlags{};
    prevPlayerHP_      = -1.0f;
    endingTimer_       = 0.0f;
    endingFadeStarted_ = false;
    groanCooldown_     = 0.0f;
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

    //SHOT FEEL: floating score + a kill sound, stronger for a headshot(撃った手応え: 浮かぶスコアとキル音。ヘッドショットは強め)
    popups_.Add(kill.position, kill.score, kill.headshot);

    //STATS for the result screen(結果画面用の集計)
    kills_++;
    if(kill.headshot)
        headshots_++;
    Sound::Play(kill.headshot ? sfxHeadshot_ : sfxKill_, Config::SFX_KILL_VOLUME);
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

        //milestones: the counter is the story, so say it out loud(節目: カウンターが物語なので無線で伝える)
        if(!said_.civilian_half && civilianSaved_ >= Config::CIVILIAN_TOTAL / 2) {
            said_.civilian_half = true;
            radio_->Say(Story::RADIO_CIVILIAN_HALF);
        }
        if(!said_.civilian_almost && civilianSaved_ >= Config::CIVILIAN_TOTAL - Config::CIVILIAN_ALMOST_LEFT) {
            said_.civilian_almost = true;
            radio_->Say(Story::RADIO_CIVILIAN_ALMOST);
        }
    }
}

void SniperVsZombie_MainStage::OnGateDamaged(float hp, float max_hp)
{
    if(isGameOver_ || max_hp <= 0.0f)
        return;

    const float ratio = hp / max_hp;

    Sound::Play(sfxGateHit_, Config::SFX_GATE_VOLUME);

    //only the most important new warning, so 2 lines never come at once(一番重要な新しい警告だけ。2つ同時に出さない)
    if(!said_.gate_low && ratio <= Config::GATE_WARN_LOW) {
        said_.gate_low = said_.gate_half = said_.gate_hit = true;
        radio_->Say(Story::RADIO_GATE_LOW);
    }
    else if(!said_.gate_half && ratio <= Config::GATE_WARN_HALF) {
        said_.gate_half = said_.gate_hit = true;
        radio_->Say(Story::RADIO_GATE_HALF);
    }
    else if(!said_.gate_hit) {
        said_.gate_hit = true;
        radio_->Say(Story::RADIO_GATE_FIRST_HIT);
    }
}

void SniperVsZombie_MainStage::OnZombieSpawned(ZombieType type)
{
    //SOUND: every stalker screeches, normal zombies groan with a cooldown(音: ストーカーは毎回叫ぶ。通常ゾンビはクールダウン付きでうめく)
    if(type == ZombieType::Stalker) {
        Sound::Play(sfxStalker_, Config::SFX_ZOMBIE_VOLUME);
    }
    else if(groanCooldown_ <= 0.0f) {
        Sound::Play(sfxZombieGroan_, Config::SFX_ZOMBIE_VOLUME);
        groanCooldown_ = Config::ZOMBIE_GROAN_COOLDOWN;
    }

    if(type == ZombieType::Stalker && !said_.stalker) {
        said_.stalker = true;
        radio_->Say(Story::RADIO_STALKER);
    }
}

void SniperVsZombie_MainStage::UpdatePlayerRadio()
{
    auto stats = playerStats_.lock();
    if(!stats)
        return;

    const float hp   = stats->GetHP();
    const bool  hurt = prevPlayerHP_ >= 0.0f && hp < prevPlayerHP_;    //HP went down this frame(このフレームでHPが減った)
    prevPlayerHP_    = hp;

    if(hurt)
        Sound::Play(sfxPlayerHurt_, Config::SFX_HURT_VOLUME);

    if(!hurt || stats->IsDead())
        return;

    if(!said_.player_low && stats->GetHPRatio() <= Config::PLAYER_WARN_LOW) {
        said_.player_low = said_.player_hit = true;
        radio_->Say(Story::RADIO_PLAYER_LOW);
    }
    else if(!said_.player_hit) {
        said_.player_hit = true;
        radio_->Say(Story::RADIO_PLAYER_HIT);
    }
}

float SniperVsZombie_MainStage::GetStageProgress() const
{
    return std::clamp(static_cast<float>(civilianSaved_) / static_cast<float>(Config::CIVILIAN_TOTAL), 0.0f, 1.0f);
}

void SniperVsZombie_MainStage::OnGameOver(GameResult result)
{
    isGameOver_        = true;
    isWin_             = result == GameResult::Win;
    result_            = result;
    stageState_        = StageState::Ending;
    endingTimer_       = Config::ENDING_HOLD_TIME;
    endingFadeStarted_ = false;

    //FREEZE: zombies stop where they are, the player can not shoot anymore (the gun stays visible)(停止: ゾンビはその場で止まり、射撃不可。銃は見えたまま)
    spawner_.SetAllPaused(true);
    if(auto camera = playerCamera_.lock())
        camera->SetStatus(Object::StatusBit::NoUpdate, true);

    switch(result) {
    case GameResult::Win:
        radio_->Say(Story::RADIO_END_WIN, true);
        break;

    case GameResult::GateBreached:
        //play the broken gate only when zombies broke it, not when the player died(ゲートが壊された時だけ壊れ演出。プレイヤー死亡では出さない)
        if(auto gate = Scene::Object::Get<Gate>(Names::GATE))
            gate->DestroyGate();
        Sound::Play(sfxGateBreak_, Config::SFX_GATE_VOLUME);
        radio_->Say(Story::RADIO_END_GATE, true);
        break;

    case GameResult::SniperDown:
        radio_->Say(Story::RADIO_END_SNIPER, true);    //the screen goes red in PostDraw(PostDrawで画面が赤くなる)
        break;
    }
}

void SniperVsZombie_MainStage::SetPaused(bool paused)
{
    if(paused_ == paused)
        return;
    paused_ = paused;

    auto camera     = playerCamera_.lock();
    auto controller = camera ? camera->GetComponent<ComponentFPSCameraController>() : nullptr;

    //give the cursor back BEFORE the controller is frozen(コントローラーを止める前にカーソルを返す)
    if(paused && controller)
        controller->OnGamePaused(true);

    //IsPause = no Update but still drawn, so the world freezes on screen. the menu and the fader must keep working(IsPause=Updateしないが描画はする。画面上で世界が止まる。メニューとフェードは動かす)
    const Object* fader = blackScreen_ ? blackScreen_->GetOwner() : nullptr;
    for(auto& obj : Scene::Object::GetArray<Object>()) {
        if(!obj || obj.get() == fader || pauseMenu_.Owns(obj.get()))
            continue;
        obj->SetStatus(Object::StatusBit::IsPause, paused);
    }

    pauseMenu_.SetOpen(paused);

    if(!paused) {
        if(controller)
            controller->OnGamePaused(false);
        //the click on RESUME must not also fire the rifle(再開ボタンのクリックで銃を撃たないように)
        if(auto shooter = std::dynamic_pointer_cast<Camera>(camera))
            shooter->DelayNextShot(0.25f);
    }
}

void SniperVsZombie_MainStage::LeaveFromPause(bool restart)
{
    if(leaving_)
        return;
    leaving_ = true;

    pauseMenu_.SetOpen(false);    //saves the settings, the world stays frozen during the fade(設定を保存。フェード中も世界は止まったまま)
    blackScreen_->StartFadeOut([restart]() {
        if(restart) {
            //GetScene would hand back THIS running stage (already initialized, so Init would not run again).(GetSceneは今動いているこのステージを返してしまう(初期化済みなのでInitがもう一度走らない))
            //forget it by name first, then GetScene makes a brand-new stage, like a restart from GameOver(先に名前で手放せば、GetSceneが新しいステージを作る。GameOverからのやり直しと同じ)
            Scene::ReleaseScene(Scene::GetScene<SniperVsZombie_MainStage>());
            Scene::Change(Scene::GetScene<SniperVsZombie_MainStage>());
        }
        else
            Scene::Change(Scene::GetScene<SniperVsZombie_MainMenu>());
    });
}

void SniperVsZombie_MainStage::OnSettingChanged(SettingsSliders::Kind kind, float value)
{
    switch(kind) {
    case SettingsSliders::Kind::Sensitivity:
        if(auto camera = playerCamera_.lock())
            if(auto controller = camera->GetComponent<ComponentFPSCameraController>())
                controller->SetSensitivityScale(value);
        break;
    case SettingsSliders::Kind::Bgm:
        Sound::PlayBgm(bgm_, Config::BGM_VOLUME);    //same call again = new volume, no restart(同じ呼び出し=音量だけ変わる)
        break;
    case SettingsSliders::Kind::Sfx:
        break;    //read every time a sound plays(音を鳴らすたびに読まれる)
    }
}

void SniperVsZombie_MainStage::UpdateEnding()
{
    endingTimer_ -= GetDeltaTime();
    if(endingTimer_ > 0.0f || endingFadeStarted_)
        return;

    endingFadeStarted_ = true;

    // capture the result + the stats AS THEY ARE RIGHT NOW into the lambda, so the callback
    // still has the correct values later even if they change in the meantime
    blackScreen_->StartFadeOut([result = result_, stats = CollectStats()]() {
        auto gameOverScene = Scene::GetScene<SniperVsZombie_GameOver>();
        gameOverScene->SetResult(result, stats);
        Scene::Change(gameOverScene);
    });
}

StageStats SniperVsZombie_MainStage::CollectStats() const
{
    StageStats stats;
    stats.score           = score_;
    stats.kills           = kills_;
    stats.headshots       = headshots_;
    stats.civilians_saved = civilianSaved_;
    stats.civilians_total = Config::CIVILIAN_TOTAL;
    stats.gate_hp_ratio   = gateMaxHP_ > 0.0f ? std::clamp(gateHP_ / gateMaxHP_, 0.0f, 1.0f) : 0.0f;
    stats.play_time       = playTime_;
    if(auto shooter = std::dynamic_pointer_cast<Camera>(playerCamera_.lock()))
        stats.shots = shooter->GetShotsFired();
    return stats;
}

void SniperVsZombie_MainStage::Update()
{
    const bool click_started = IsClickStarted();

    //PAUSE (P / Tab) - only during normal play. while paused nothing below runs, so timers and spawns stop too(ポーズ。通常プレイ中のみ。ポーズ中は以下が動かないので、タイマーや出現も止まる)
    if(stageState_ == StageState::Playing && !leaving_ && (IsKeyDown(PauseMenu::KEY_1) || IsKeyDown(PauseMenu::KEY_2)))
        SetPaused(!paused_);
    if(paused_)
        return;

    //OPENING - nothing moves, a click skips the cutscene(オープニング。何も動かない。クリックでスキップ)
    if(stageState_ == StageState::Opening) {
        if(click_started && opening_)
            opening_->Skip();
        return;
    }

    //ZOMBIES - no new zombies once the game is over(ゾンビ。ゲーム終了後は新しく出さない)
    if(!isGameOver_)
        spawner_.Update(GetDeltaTime(), GetStageProgress());

    //Update civilians count. stops at the end, so the counter does not change after the result(市民数を更新。終了後は結果が変わらないよう止める)
    if(!isGameOver_)
        UpdateCivilian();

    //Get gate HP
    if(auto gate = Scene::Object::Get<Gate>(Names::GATE)) {
        gateHP_ = gate->GetHP();
    }

    //PLAY TIME for the result screen. the pause already returned above, so it is not counted(結果画面用のプレイ時間。ポーズは上でreturn済みなので数えない)
    if(!isGameOver_)
        playTime_ += GetDeltaTime();

    if(groanCooldown_ > 0.0f)
        groanCooldown_ -= GetDeltaTime();

    //ENDING - wait, then fade to GameOver(終了。待ってからGameOverへフェード)
    if(stageState_ == StageState::Ending)
        UpdateEnding();

    //player hurt radio lines(プレイヤー被弾の無線)
    if(!isGameOver_)
        UpdatePlayerRadio();

    //Check Gameover
    if(!isGameOver_) {
        bool isPlayerDead = false;
        if(auto stats = playerStats_.lock())
            isPlayerDead = stats->IsDead();

        if(gateHP_ <= 0)
            OnGameOver(GameResult::GateBreached);    //lose, gate broke(負け。ゲートが壊れた)
        else if(isPlayerDead)
            OnGameOver(GameResult::SniperDown);    //lose, player died(負け。プレイヤーが死んだ)
        else if(civilianSaved_ >= Config::CIVILIAN_TOTAL)
            OnGameOver(GameResult::Win);    //win(勝ち)
    }

    //Hit marker UI
    if(hitmarkerAlpha_ > 0)
        hitmarkerAlpha_ -= 3;

    //GATE ARROW INDICATOR - tick the bounce timer every frame
    arrowTimer_ += GetDeltaTime();

    popups_.Update(GetDeltaTime());
}

void SniperVsZombie_MainStage::Exit()
{
    DeleteEffekseerEffect(hitEffectHandle_);

    Sound::Release(bgm_);
    Sound::Release(sfxGateHit_);
    Sound::Release(sfxGateBreak_);
    Sound::Release(sfxZombieGroan_);
    Sound::Release(sfxStalker_);
    Sound::Release(sfxPlayerHurt_);
    Sound::Release(sfxKill_);
    Sound::Release(sfxHeadshot_);
    Hud::ReleaseFont();    //free the HUD font, it is made again next time it is needed(HUDフォントを解放。次に必要な時に作り直される)
}

void SniperVsZombie_MainStage::PostDraw()
{
    __super::PostDraw();

    const int          line  = Hud::GetLineHeight();
    const unsigned int white = GetColor(255, 255, 255);

    //OPENING - no HUD, only the fade cuts + skip hint(オープニング。HUDなし、フェードカットとスキップ案内だけ)
    if(stageState_ == StageState::Opening) {
        if(opening_)
            opening_->DrawFade();
        Hud::DrawLabelRight(WINDOW_W - 20, WINDOW_H - 20 - line, GetColor(200, 200, 200), "%s", Story::OPENING_SKIP);
        blackScreen_->DrawBlackScreen();
        return;
    }

    //----------------------------------------------------------
    // HUD - all text uses Hud:: = bigger font with a black outline(HUD - 文字は全部Hud::を使う=大きめで黒い縁取り付き)
    //----------------------------------------------------------

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

    //RELOAD BAR under the crosshair: shrinks while the bolt is pulled, gone when you can shoot(照準の下の装填バー: ボルトを引いている間に縮み、撃てる時は消える)
    if(stageState_ == StageState::Playing) {
        if(auto camera = Scene::Object::Get<Camera>()) {
            const float ratio = camera->GetReloadRatio();
            if(ratio > 0.0f) {
                const float half_w = 40.0f;    //half bar width, pixels(バーの幅の半分・ピクセル)
                const float cx     = WINDOW_W * 0.5f;
                const float top    = WINDOW_H * 0.5f + 34.0f;    //a bit under the crosshair(照準の少し下)
                const float bottom = top + 5.0f;
                DrawBoxAA(cx - half_w - 1.0f, top - 1.0f, cx + half_w + 1.0f, bottom + 1.0f, GetColor(0, 0, 0), TRUE);    //black back(黒い背景)
                DrawBoxAA(
                    cx - half_w * ratio, top, cx + half_w * ratio, bottom, GetColor(240, 180, 76), TRUE);    //amber, shrinks to the center(琥珀色。中央へ縮む)
            }
        }
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

    //STALKER WARNING: red arrow on it, or at the screen edge when it is off screen(ストーカー警告: 敵の上、または画面外なら画面端に赤い矢印)
    if(stageState_ == StageState::Playing) {
        for(const float3& pos : spawner_.GetLivingPositions(ZombieType::Stalker)) Hud::DrawThreatIndicator(pos, arrowTimer_);
    }

    //SCORE POPUPS at the kill spots(キル地点のスコア表示)
    popups_.Draw();

    //PLAYER HP UI + red hit flash(プレイヤーHP表示と被弾の赤フラッシュ)
    if(auto stats = playerStats_.lock())
        stats->DrawHUD();

    //SNIPER DOWN - the screen slowly turns red during the ending hold(狙撃手ダウン。終了待ちの間に画面がゆっくり赤くなる)
    if(stageState_ == StageState::Ending && result_ == GameResult::SniperDown && Config::ENDING_HOLD_TIME > 0.0f) {
        const float t = std::clamp(1.0f - endingTimer_ / Config::ENDING_HOLD_TIME, 0.0f, 1.0f);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(Config::SNIPER_DOWN_RED_ALPHA * t));
        DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(120, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    //RADIO subtitles, then the short fade after a skipped opening(無線字幕、その後スキップ後の短いフェード)
    radio_->DrawRadio();

    //PAUSE MENU on top of the HUD(HUDの上にポーズメニュー)
    pauseMenu_.Draw();
    if(opening_)
        opening_->DrawFade();

    //BLACK SCREEN TRANSITION - draw LAST so it covers everything above during fades
    blackScreen_->DrawBlackScreen();

    //the big result text moved to GameOver: it shows after the ending cutscene now(大きな結果の文字はGameOverへ移動: エンディングのカットシーン後に出る)
}

}    // namespace SniperVsZombie
