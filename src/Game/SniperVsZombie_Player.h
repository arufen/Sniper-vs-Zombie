//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Player.h
//! @brief	SniperVsZombie_Player
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Player);

//thin object: only the hit body + stats. camera, gun and shooting live in Camera(薄いObject。当たり判定とステータスだけ。カメラ・銃・射撃はCameraにある)
class Player : public Object
{
public:
    BP_OBJECT_DECL(Player, "SniperVsZombie::Player");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
