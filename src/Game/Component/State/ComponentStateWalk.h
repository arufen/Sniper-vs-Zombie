#pragma once
#include <System/Scene.h>
#include <Game/Component/State/ComponentState.h>

USING_PTR(ComponentStateWalk);

class ComponentStateWalk : public ComponentState
{
public:
    BP_COMPONENT_DECL(ComponentStateWalk, u8"停止・歩き");

    void Init() override;

    void Update() override;

    ComponentStateWalkPtr SetMoveSpeed(const float speed);

    ComponentStateWalkPtr SetRotateSpeed(const float speed);

    const float GetMoveSpeed() const;
    const float GetRotateSpeed() const;

    inline const void  SetFrontRotate(float rotate) { front_rot_ = rotate; }
    inline const float GetFrontRotate() const { return front_rot_; }

    void GUI() override;

    void SetTargetName(const std::string_view name) { target_name_ = name; }

private:
    float move_speed_ = 0.2f;     //!< 移動スピード
    float rot_speed_  = 20.0f;    //!< 回転スピード

    float front_rot_ = 0.0f;    //!<前方ベクトルの回転角度(0-360度)

    std::string target_name_ = "Player";

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //! @brief セーブ
    // @param arc アーカイバ
    // @param ver バージョン
    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(move_speed_),
            CEREAL_NVP(rot_speed_),

            CEREAL_NVP(front_rot_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentStateWalk, 1);
