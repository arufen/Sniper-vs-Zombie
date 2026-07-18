//---------------------------------------------------------------------------
//!	@file	TutorialX_Camera.h
//! @brief	TutorialX_Camera
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Camera);
class Camera : public Object
{
public:
    BP_OBJECT_DECL(Camera, "SniperVsZombie::Camera");

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
};
}    // namespace SniperVsZombie
