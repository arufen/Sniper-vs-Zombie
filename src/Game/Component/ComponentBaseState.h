#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <Game/Component/State/ComponentState.h>

USING_PTR(ComponentBaseState);

class ComponentBaseState : public Component
{
public:
    BP_COMPONENT_DECL(ComponentBaseState, u8"ステートコントロール(Base)");

    void Init() override;

    void Update() override;

    void GUI() override;

    //! @brief 現在のステートを取得する
    const std::string_view& GetStateName() const
    {
        if(auto cmp = GetOwner()->GetComponent<ComponentBaseState>()) {
            return cmp->GetName();
        }
    }

    //! @brief 次のステートにする
    template <typename T>
    bool ChangeState()
    {
        auto owner = GetOwner();
        owner->RemoveComponent<ComponentBaseState>();
        owner->AddComponent<T>();
    }

    template <class T>
    const bool IsState() const
    {
        return GetOwner()->GetComponent<T>() ? true : false;
    }

private:
    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //! @brief セーブ
    // @param arc アーカイバ
    // @param ver バージョン
    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};
