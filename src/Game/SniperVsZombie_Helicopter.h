//---------------------------------------------------------------------------
//!	@file	TutorialX_Helicopter.h
//! @brief	TutorialX_Helicopter
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Helicopter);
class Helicopter : public Object
{
public:
    BP_OBJECT_DECL(Helicopter, "SniperVsZombie::Helicopter");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
