//---------------------------------------------------------------------------
//!	@file	TutorialX_Sky.h
//! @brief	TutorialX_Sky
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Sky);
class Sky : public Object
{
public:
    BP_OBJECT_DECL(Sky, "SniperVsZombie::Sky");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
