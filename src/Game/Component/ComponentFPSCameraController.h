#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>

USING_PTR(ComponentFPSCameraController);

class ComponentFPSCameraController : public Component
{
public:
    BP_COMPONENT_DECL(ComponentFPSCameraController, u8"FPSカメラコンポーネント");

    void Init() override;

    void Update() override;

    void LateDraw() override;

    ComponentFPSCameraControllerPtr SetEyePosition(float3 pos);

    ComponentFPSCameraControllerPtr SetMouseSensitivity(float sensitivity);

    ComponentFPSCameraControllerPtr SetPitchLimit(float min_deg, float max_deg);

    ComponentFPSCameraControllerPtr SetFov(float fov_deg);

    float3 GetForwardVector(float offset = 1.0f) const;

    void GUI() override;

private:
    const float DEFAULT_MOUSE_SENSITIVITY_ = 0.005f;    //!< デフォルトのマウス感度

    float zoom_progress_ = 0.0f;    // 0 = normal, 1 = fully zoomed
    float zoom_duration_ = 0.1f;    // how long zoom takes (seconds)

    float3 eye_position_ = {0.0f, 1.5f, 0.0f};    //!< カメラのオフセット位置

    float rotate_v_ = 0.0f;    //!< 上下回転(ラジアン)
    float rotate_h_ = 0.0f;    //!< 左右回転(ラジアン)

    float mouse_sensitivity_ = DEFAULT_MOUSE_SENSITIVITY_;    //!< マウス感度

    float limit_pitch_min_ = -60.0f * DegToRad;    //!< 下限
    float limit_pitch_max_ = +89.0f * DegToRad;    //!< 上限

    float fov_ = 60.0f * DegToRad;    //!< 画角

    bool isZoom_ = false;    //!< ズーム中かどうか

    int scopeHandle;

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(eye_position_),
            CEREAL_NVP(rotate_v_),
            CEREAL_NVP(rotate_h_),
            CEREAL_NVP(mouse_sensitivity_),
            CEREAL_NVP(limit_pitch_min_),
            CEREAL_NVP(limit_pitch_max_),
            CEREAL_NVP(fov_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentFPSCameraController, 1);
