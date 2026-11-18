//---------------------------------------------------------------------------
//!	@file	SoundUtil.h
//! @brief	safe sound helpers: a missing file never crashes the game(安全なサウンド関数: ファイルが無くてもゲームは落ちない)
//---------------------------------------------------------------------------
#pragma once

namespace SniperVsZombie::Sound {

//load a sound. returns -1 if the file does not exist yet (assets come later, see RequiredAssets.md)(音を読み込む。ファイルがまだ無ければ-1。素材は後で追加、RequiredAssets.md参照)
int Load(const char* path);

//play once. volume 0.0 - 1.0. does nothing for -1(1回再生。音量0.0〜1.0。-1なら何もしない)
void Play(int handle, float volume = 1.0f);

//play forever (BGM, helicopter). does nothing for -1(ループ再生。BGMやヘリ用。-1なら何もしない)
void PlayLoop(int handle, float volume = 1.0f);

void Stop(int handle);

//stop + free, and set the handle back to -1(停止+解放して、ハンドルを-1に戻す)
void Release(int& handle);

//true if the file is really there (also used for models)(ファイルが本当にあればtrue。モデルにも使う)
bool FileExists(const char* path);

}    // namespace SniperVsZombie::Sound
