//---------------------------------------------------------------------------
//!	@file	SoundUtil.cpp
//! @brief	safe sound helpers(安全なサウンド関数)
//---------------------------------------------------------------------------
#include "SoundUtil.h"
#include <System/Scene.h>
#include <algorithm>
#include <filesystem>

namespace SniperVsZombie::Sound {

namespace {
int ToDxVolume(float volume)
{
    return static_cast<int>(255.0f * std::clamp(volume, 0.0f, 1.0f));    //DxLib volume is 0-255(DxLibの音量は0〜255)
}
}    // namespace

bool FileExists(const char* path)
{
    std::error_code error;    //no exception if the path is strange(パスがおかしくても例外を出さない)
    return std::filesystem::exists(path, error);
}

int Load(const char* path)
{
    if(!FileExists(path))
        return -1;
    return LoadSoundMem(path);
}

void Play(int handle, float volume)
{
    if(handle == -1)
        return;
    ChangeVolumeSoundMem(ToDxVolume(volume), handle);
    PlaySoundMem(handle, DX_PLAYTYPE_BACK);    //BACK = do not wait, overlapping plays are fine(BACK=待たない。重ねて鳴らしてもOK)
}

void PlayLoop(int handle, float volume)
{
    if(handle == -1)
        return;
    ChangeVolumeSoundMem(ToDxVolume(volume), handle);
    if(CheckSoundMem(handle) != 1)    //do not restart it if it is already playing(すでに再生中なら最初からにしない)
        PlaySoundMem(handle, DX_PLAYTYPE_LOOP);
}

void Stop(int handle)
{
    if(handle == -1)
        return;
    StopSoundMem(handle);
}

void Release(int& handle)
{
    if(handle == -1)
        return;
    StopSoundMem(handle);
    DeleteSoundMem(handle);
    handle = -1;
}

}    // namespace SniperVsZombie::Sound
