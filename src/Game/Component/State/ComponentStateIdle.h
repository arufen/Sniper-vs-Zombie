#pragma once
#include <System/Scene.h>
#include <Game/Component/State/ComponentState.h>

USING_PTR(ComponentStateIdle);

class ComponentStateIdle : public ComponentState
{
public:
    BP_COMPONENT_DECL(ComponentStateIdle, u8"停止・AI停止");

    void Init() override;

    void Update() override;

    void GUI() override;

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

CEREAL_CLASS_VERSION(ComponentStateIdle, 1);
