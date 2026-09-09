//---------------------------------------------------------------------------
//!	@file	TutorialX_Gate.h
//! @brief	TutorialX_Gate
//---------------------------------------------------------------------------
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(Gate);
class Gate : public Object
{
public:
    BP_OBJECT_DECL(Gate, "SniperVsZombie::Gate");
    void  TakeDamage(float damage);
    void  DestroyGate();
    float GetHP() const { return HP_; }
    float GetMaxHP() const { return maxHP_; }
    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;

private:
    int maxHP_ = 100;
    int HP_    = maxHP_;
};
}    // namespace SniperVsZombie
