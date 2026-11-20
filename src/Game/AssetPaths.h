//---------------------------------------------------------------------------
//!	@file	AssetPaths.h
//! @brief	paths of the story assets (sounds, models). keep it in sync with AI Context/RequiredAssets.md(ストーリー用素材のパス。RequiredAssets.mdと合わせる)
//---------------------------------------------------------------------------
#pragma once

//missing files are OK: sounds load as -1 (silent), models are skipped. see SoundUtil.h(ファイルが無くてもOK: 音は-1(無音)、モデルは作らない)
namespace SniperVsZombie::Assets {

//SFX(効果音)
inline constexpr const char* SFX_RADIO_BEEP      = "data/Game/Audio/sfx_radio_beep.mp3";
inline constexpr const char* SFX_GATE_HIT        = "data/Game/Audio/sfx_gate_hit.mp3";
inline constexpr const char* SFX_GATE_BREAK      = "data/Game/Audio/sfx_gate_break.mp3";
inline constexpr const char* SFX_ZOMBIE_GROAN    = "data/Game/Audio/sfx_zombie_groan.mp3";
inline constexpr const char* SFX_STALKER_SCREECH = "data/Game/Audio/sfx_stalker_screech.mp3";
inline constexpr const char* SFX_HELI_ROTOR_LOOP = "data/Game/Audio/sfx_heli_rotor_loop.mp3";
inline constexpr const char* SFX_PLAYER_HURT     = "data/Game/Audio/sfx_player_hurt.mp3";
inline constexpr const char* SFX_KILL_CONFIRM    = "data/Game/Audio/sfx_kill_confirm.mp3";    //body kill, short "tick"(胴体キル。短い「カチッ」)
inline constexpr const char* SFX_HEADSHOT        = "data/Game/Audio/sfx_headshot.mp3";        //headshot kill, stronger(ヘッドショットキル。強め)

//BGM
inline constexpr const char* BGM_MENU  = "data/Game/Audio/bgm_menu.mp3";
inline constexpr const char* BGM_STAGE = "data/Game/Audio/bgm_stage.mp3";
inline constexpr const char* BGM_WIN   = "data/Game/Audio/bgm_win.mp3";
inline constexpr const char* BGM_LOSE  = "data/Game/Audio/bgm_lose.mp3";

//MODELS(モデル) - the new player model, used in the "sniper down" cutscene(新しいプレイヤーモデル。「狙撃手ダウン」のカットシーン用)
inline constexpr const char* PLAYER_MODEL      = "data/Game/Models/Player/Player.mv1";
inline constexpr const char* PLAYER_ANIM_DEATH = "data/Game/Models/Player/Anims/Death.mv1";

//zombie model for cutscenes (already exists)(カットシーン用ゾンビモデル。既にある)
inline constexpr const char* ZOMBIE_MODEL       = "data/Game/Models/Enemy/Enemy.mv1";
inline constexpr const char* ZOMBIE_ANIM_ATTACK = "data/Game/Models/Enemy/Anims/Attack.mv1";

}    // namespace SniperVsZombie::Assets
