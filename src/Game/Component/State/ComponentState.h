#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>

USING_PTR(ComponentState);

class ComponentState : public Component
{
public:
    BP_COMPONENT_DECL(ComponentState, u8"ベースState");

    void Init() override;

    void Update() override;

    void GUI() override;

    //! @brief 次のステートにする
    template <typename T>
    const void ChangeState()
    {
        RemoveThisComponent();
        GetOwner()->AddComponent<T>();
    }

    //! @brief アニメーションが終わったら次のステートにする
    template <typename T>
    const void ChangeStateWhenAnimEnd()
    {
        if(auto model = GetOwner()->GetComponent<ComponentModel>()) {
            if(!model->IsPlaying()) {
                ChangeState<T>();
            }
        }
    }

    //! @brief アニメーション時間の取得
    //! @return 取れない場合は 0.0とする
    const float GetAnimationTime() const
    {
        if(auto model = GetOwner()->GetComponent<ComponentModel>()) {
            return model->GetAnimationTime();
        }
        return 0.0f;
    }

    const void ChangeAnimation(const std::string_view name, bool loop = false)
    {
        if(auto model = GetOwner()->GetComponent<ComponentModel>()) {
            model->PlayAnimationNoSame(name, loop);
        }
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
