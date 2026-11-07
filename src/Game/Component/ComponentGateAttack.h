#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <string>
#include <string_view>

USING_PTR(ComponentGateAttack);

class ComponentGateAttack : public Component
{
public:
    BP_COMPONENT_DECL(ComponentGateAttack, u8"ゲート攻撃");

    void Init() override;

    void Update() override;

    void GUI() override;

    //name of the object to attack, default "Gate"(攻撃する相手のObject名。デフォルトは"Gate")
    ComponentGateAttackPtr SetTargetName(std::string_view name);

    //how close to the target before attacking(どこまで近づいたら攻撃するか)
    ComponentGateAttackPtr SetAttackRange(float range);

    //seconds between hits. the first hit comes earlier, at 25% of this(攻撃の間隔・秒。最初の一撃は25%で早め)
    ComponentGateAttackPtr SetAttackInterval(float interval);

    //damage per hit(1回のダメージ)
    ComponentGateAttackPtr SetDamage(float damage);

    bool IsAttacking() const { return is_attacking_; }

private:
    std::string target_name_ = "Gate";    //target object name(攻撃対象のObject名)

    float attack_range_    = 50.0f;                       //attack distance(攻撃距離)
    float attack_interval_ = 5.0f;                        //seconds between hits(攻撃の間隔)
    float attack_time_     = attack_interval_ * 0.25f;    //time until the next hit, starts early(次の攻撃までの時間。最初は早め)
    float damage_          = 10.0f;                       //damage per hit(1回のダメージ)

    bool is_attacking_ = false;    //true while in range(射程内の間true)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(attack_range_), CEREAL_NVP(attack_interval_), CEREAL_NVP(damage_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentGateAttack, 1);
