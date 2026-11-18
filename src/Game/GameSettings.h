//---------------------------------------------------------------------------
//!	@file	GameSettings.h
//! @brief	player settings that are saved to a file (mouse sensitivity, ...)(ファイルに保存されるプレイヤー設定(マウス感度など))
//---------------------------------------------------------------------------
#pragma once

namespace SniperVsZombie::Settings {

//mouse sensitivity is a MULTIPLIER: 1.0 = the normal speed, 2.0 = twice as fast(マウス感度は倍率: 1.0=通常、2.0=2倍の速さ)
constexpr float MOUSE_SENSITIVITY_MIN     = 0.2f;
constexpr float MOUSE_SENSITIVITY_MAX     = 3.0f;
constexpr float MOUSE_SENSITIVITY_DEFAULT = 1.0f;
constexpr float MOUSE_SENSITIVITY_STEP    = 0.05f;    //the slider snaps to this(スライダーはこの単位で止まる)

//the file is read the first time a Get is called, so no Init call is needed(最初のGetでファイルを読むので、初期化の呼び出しは不要)
float GetMouseSensitivity();

//clamped to MIN..MAX. call Save() to keep it after the game closes(MIN..MAXに収める。ゲーム終了後も残すにはSave()を呼ぶ)
void SetMouseSensitivity(float value);

//write every setting to data/_save/settings.txt (that folder is in .gitignore)(全設定をdata/_save/settings.txtに書く。このフォルダは.gitignore済み)
void Save();

}    // namespace SniperVsZombie::Settings
