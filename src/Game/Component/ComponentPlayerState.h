#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <Game/Component/ComponentState.h>

USING_PTR(ComponentPlayerState);

class ComponentPlayerState : public Component
{
public:
    BP_COMPONENT_DECL(ComponentPlayerState, u8"ステートコントロール");

    void Init() override;

    void Update() override;

    void GUI() override;

    //! @brief 現在のステートを取得する
    const std::string_view& GetStateName() const
    {
        if(auto cmp = GetOwner()->GetComponent<ComponentState>()) {
            return cmp->GetName();
        }
    }

    //! @brief 次のステートにする
    template <typename T>
    bool ChangeState()
    {
        auto owner = GetOwner();
        owner->RemoveComponent<ComponentState>();
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

CEREAL_CLASS_VERSION(ComponentPlayerState, 1);
