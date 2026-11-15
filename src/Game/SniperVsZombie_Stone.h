//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Stone.h
//! @brief	SniperVsZombie_Stone
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Stone);
class Stone : public Object
{
public:
    BP_OBJECT_DECL(Stone, "SniperVsZombie::Stone");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
