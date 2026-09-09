//---------------------------------------------------------------------------
//!	@file	TutorialX_City.h
//! @brief	TutorialX_City
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(City);
class City : public Object
{
public:
    BP_OBJECT_DECL(City, "SniperVsZombie::City");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
