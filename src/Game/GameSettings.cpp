//---------------------------------------------------------------------------
//!	@file	GameSettings.cpp
//! @brief	player settings that are saved to a file(ファイルに保存されるプレイヤー設定)
//---------------------------------------------------------------------------
#include "GameSettings.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

namespace SniperVsZombie::Settings {

namespace {
const char* SAVE_FOLDER = "data/_save";
const char* SAVE_FILE   = "data/_save/settings.txt";

bool  loaded            = false;
float mouse_sensitivity = MOUSE_SENSITIVITY_DEFAULT;
float bgm_volume        = VOLUME_DEFAULT;
float sfx_volume        = VOLUME_DEFAULT;

//file format: one "key=value" per line, easy to read and edit by hand(形式: 1行に「key=value」。手で読み書きしやすい)
void LoadOnce()
{
    if(loaded)
        return;
    loaded = true;

    std::ifstream file(SAVE_FILE);
    std::string   line;
    while(std::getline(file, line)) {    //no file = nothing happens, the defaults stay(ファイルが無ければ何もしない。初期値のまま)
        const size_t eq = line.find('=');
        if(eq == std::string::npos)
            continue;

        const std::string key   = line.substr(0, eq);
        const std::string value = line.substr(eq + 1);
        try {
            if(key == "mouse_sensitivity")
                mouse_sensitivity = std::clamp(std::stof(value), MOUSE_SENSITIVITY_MIN, MOUSE_SENSITIVITY_MAX);
            else if(key == "bgm_volume")
                bgm_volume = std::clamp(std::stof(value), VOLUME_MIN, VOLUME_MAX);
            else if(key == "sfx_volume")
                sfx_volume = std::clamp(std::stof(value), VOLUME_MIN, VOLUME_MAX);
        }
        catch(...) {
            //broken number in the file: keep the default(ファイルの数字が壊れている: 初期値のまま)
        }
    }
}
}    // namespace

float GetMouseSensitivity()
{
    LoadOnce();
    return mouse_sensitivity;
}

void SetMouseSensitivity(float value)
{
    LoadOnce();
    mouse_sensitivity = std::clamp(value, MOUSE_SENSITIVITY_MIN, MOUSE_SENSITIVITY_MAX);
}

float GetBgmVolume()
{
    LoadOnce();
    return bgm_volume;
}

void SetBgmVolume(float value)
{
    LoadOnce();
    bgm_volume = std::clamp(value, VOLUME_MIN, VOLUME_MAX);
}

float GetSfxVolume()
{
    LoadOnce();
    return sfx_volume;
}

void SetSfxVolume(float value)
{
    LoadOnce();
    sfx_volume = std::clamp(value, VOLUME_MIN, VOLUME_MAX);
}

void Save()
{
    LoadOnce();

    std::error_code error;
    std::filesystem::create_directories(SAVE_FOLDER, error);    //make the folder the first time(初回はフォルダを作る)

    std::ofstream file(SAVE_FILE, std::ios::trunc);
    if(!file)
        return;    //can't write (read-only folder etc.): the setting still works until the game closes(書けない: ゲーム終了までは設定は有効)
    file << "mouse_sensitivity=" << mouse_sensitivity << "\n";
    file << "bgm_volume=" << bgm_volume << "\n";
    file << "sfx_volume=" << sfx_volume << "\n";
}

}    // namespace SniperVsZombie::Settings
