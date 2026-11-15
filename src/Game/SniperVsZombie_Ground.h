//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Ground.h
//! @brief	SniperVsZombie_Ground
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Ground);
class Ground : public Object
{
public:
    BP_OBJECT_DECL(Ground, "SniperVsZombie::Ground");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
