//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Gate.h
//! @brief	SniperVsZombie_Gate
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <functional>

namespace SniperVsZombie {
USING_PTR(Gate);
class Gate : public Object
{
public:
    BP_OBJECT_DECL(Gate, "SniperVsZombie::Gate");

    using OnDamagedFunc = std::function<void(float hp, float max_hp)>;

    void TakeDamage(float damage);

    //called every time the gate really takes damage, e.g. for radio lines and the hit sound(ゲートが実際にダメージを受けるたびに呼ばれる。無線や被弾音用)
    void SetOnDamaged(OnDamagedFunc on_damaged) { on_damaged_ = on_damaged; }

    void  DestroyGate();
    float GetHP() const { return HP_; }
    float GetMaxHP() const { return maxHP_; }
    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;

private:
    int maxHP_ = 100;
    int HP_    = maxHP_;

    OnDamagedFunc on_damaged_;
};
}    // namespace SniperVsZombie
