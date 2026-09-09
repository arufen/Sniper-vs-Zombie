#pragma once
#include <System/Scene.h>
#include <Game/Component/ComponentBaseState.h>
#include <Game/Component/State/ComponentState.h>

USING_PTR(ComponentAIState);

class ComponentAIState : public ComponentBaseState
{
public:
    BP_COMPONENT_DECL(ComponentAIState, u8"ステートコントロール");

    void Init() override;

    void Update() override;

    void GUI() override;

    //! @brief 次のステートにする
    template <typename T>
    bool ChangeState()
    {
        auto owner = GetOwner();
        owner->RemoveComponent<ComponentBaseState>();
        owner->AddComponent<T>();
    }

    void SetTargetName(const std::string_view name) { target_name_ = name; }

    const std::string_view GetTargetName() const { return target_name_; }

    //! @brief chase a raw point instead of a named object (e.g. a waypoint that isn't a real Object)
    void SetTargetPosition(const float3& pos)
    {
        target_position_     = pos;
        has_target_position_ = true;
    }

    //! @brief go back to chasing GetTargetName() instead of a raw point
    void ClearTargetPosition() { has_target_position_ = false; }

    bool HasTargetPosition() const { return has_target_position_; }

    const float3& GetTargetPosition() const { return target_position_; }

private:
    std::string target_name_         = "Player";
    float3      target_position_     = {};
    bool        has_target_position_ = false;    // when true, movement should chase target_position_ instead of target_name_
    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //! @brief セーブ
    // @param arc アーカイバ
    // @param ver バージョン
    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};

CEREAL_CLASS_VERSION(ComponentAIState, 1);
