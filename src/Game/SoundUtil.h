//---------------------------------------------------------------------------
//!	@file	SoundUtil.h
//! @brief	safe sound helpers: a missing file never crashes the game(安全なサウンド関数: ファイルが無くてもゲームは落ちない)
//---------------------------------------------------------------------------
#pragma once

namespace SniperVsZombie::Sound {

//load a sound. returns -1 if the file does not exist yet (assets come later, see RequiredAssets.md)(音を読み込む。ファイルがまだ無ければ-1。素材は後で追加、RequiredAssets.md参照)
int Load(const char* path);

//every volume below is multiplied by the player's setting: SFX for Play/PlayLoop, BGM for PlayBgm(以下の音量は全てプレイヤー設定倍率がかかる: Play/PlayLoopは効果音、PlayBgmは音楽)

//play once. volume 0.0 - 1.0. does nothing for -1(1回再生。音量0.0〜1.0。-1なら何もしない)
void Play(int handle, float volume = 1.0f);

//play forever, as a sound effect (helicopter). does nothing for -1(効果音としてループ再生。ヘリ用。-1なら何もしない)
void PlayLoop(int handle, float volume = 1.0f);

//play forever, as music. call it again to apply a new volume, it does not restart(音楽としてループ再生。もう一度呼ぶと音量だけ変わり、最初からにはならない)
void PlayBgm(int handle, float volume = 1.0f);

void Stop(int handle);

//stop + free, and set the handle back to -1(停止+解放して、ハンドルを-1に戻す)
void Release(int& handle);

//true if the file is really there (also used for models)(ファイルが本当にあればtrue。モデルにも使う)
bool FileExists(const char* path);

}    // namespace SniperVsZombie::Sound
