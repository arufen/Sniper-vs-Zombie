//---------------------------------------------------------------------------
//!	@file	TutorialX_WoodTower.h
//! @brief	TutorialX_WoodTower
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(WoodTower);
class WoodTower : public Object
{
public:
    BP_OBJECT_DECL(WoodTower, "SniperVsZombie::WoodTower");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
