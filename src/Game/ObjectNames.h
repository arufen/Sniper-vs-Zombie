//---------------------------------------------------------------------------
//!	@file	ObjectNames.h
//! @brief	every Object name in one place(全Object名を1か所にまとめる)
//---------------------------------------------------------------------------
#pragma once

//a typo in "Gate" compiles fine and silently finds nothing. a typo in Names::GATE is a compile error(文字列"Gate"の打ち間違いはコンパイルが通り、何も見つからないだけ。Names::GATEの打ち間違いはコンパイルエラーになる)
//use these for SetName(), Scene::Object::Get<T>() and name checks in OnHit()(SetName、Get<T>、OnHitの名前チェックにはこれを使う)
namespace SniperVsZombie::Names {

//stage objects(ステージのObject)
inline constexpr const char* GATE          = "Gate";
inline constexpr const char* GATE_CUTSCENE = "Gate_cutscene";    //GameOver only, so zombies do not target it(GameOver専用。ゾンビの標的にならないよう別名)
inline constexpr const char* CITY          = "City";
inline constexpr const char* WOOD_TOWER    = "WoodTower";
inline constexpr const char* HELICOPTER    = "Helicopter";
inline constexpr const char* GROUND        = "Ground";
inline constexpr const char* SKY           = "Sky";
inline constexpr const char* STONE         = "Stone";

//player side(プレイヤー側)
inline constexpr const char* PLAYER = "Player";
inline constexpr const char* CAMERA = "Camera";
inline constexpr const char* BULLET = "Bullet";

//zombies(ゾンビ)
inline constexpr const char* NORMAL_ZOMBIE = "NormalZombie";
inline constexpr const char* STALKER       = "Stalker";

//UI / system(UI・システム)
inline constexpr const char* BLACK_SCREEN  = "BlackScreen";
inline constexpr const char* SHADER_CAMERA = "ShaderCamera";
inline constexpr const char* RADIO         = "Radio";
inline constexpr const char* CAMERA_PROBE  = "CameraProbe";

//cutscenes(カットシーン)
inline constexpr const char* CUTSCENE_CAMERA = "CutsceneCamera";
inline constexpr const char* CUTSCENE_PLAYER = "CutscenePlayer";    //dead sniper body (GameOver)(倒れた狙撃手の体)
inline constexpr const char* CUTSCENE_STALKER =
    "CutsceneStalker";    //stalker over the body (GameOver)(体の上のストーカー)    //dev tool, Debug builds only(開発ツール。Debugビルドのみ)

}    // namespace SniperVsZombie::Names
