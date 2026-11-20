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

    void Exit() override;

    //key that turns mouse lock on/off(マウスロックを切り替えるキー)
    static constexpr int MOUSE_LOCK_KEY = KEY_INPUT_Q;

    //true = the mouse turns the camera without holding right click(true=右クリックを押さなくてもマウスでカメラが回る)
    bool IsMouseLocked() const { return mouse_locked_; }

    ComponentFPSCameraControllerPtr SetEyePosition(float3 pos);

    ComponentFPSCameraControllerPtr SetMouseSensitivity(float sensitivity);

    //player's sensitivity setting as a multiplier, 1.0 = normal. the scope zoom still slows it down on top(プレイヤーの感度設定・倍率。1.0=通常。スコープのズーム減速はその上にかかる)
    ComponentFPSCameraControllerPtr SetSensitivityScale(float scale);

    ComponentFPSCameraControllerPtr SetPitchLimit(float min_deg, float max_deg);

    ComponentFPSCameraControllerPtr SetFov(float fov_deg);

    float3 GetForwardVector(float offset = 1.0f) const;

    //look direction in degrees. x = yaw (left/right), y = pitch (+ = looking down)(視線の向き・度。x=左右、y=上下(+で下を向く))
    float2 GetLookAngles() const { return float2(rotate_h_ * RadToDeg, rotate_v_ * RadToDeg); }

    //set the look direction in degrees, e.g. where the player faces when control starts(視線の向きを度で設定。操作開始時に向く方向など)
    ComponentFPSCameraControllerPtr SetLookAngles(float yaw_deg, float pitch_deg);

    //kick the VIEW up by pitch_deg, it springs back by itself. the aim (GetForwardVector) does not move(視点を上に跳ねさせる。自動で戻る。照準(GetForwardVector)は動かない)
    void AddRecoil(float pitch_deg, float return_speed);

    //the game froze this component (pause menu): true = give the cursor back, false = forget mouse moves made during the pause(ゲームがこのコンポーネントを止めた(ポーズ): true=カーソルを返す、false=ポーズ中のマウス移動を忘れる)
    void OnGamePaused(bool paused);

    void GUI() override;

private:
    const float DEFAULT_MOUSE_SENSITIVITY_ = 0.005f;    //!< デフォルトのマウス感度

    float zoom_progress_ = 0.0f;    // 0 = normal, 1 = fully zoomed
    float zoom_duration_ = 0.1f;    // how long zoom takes (seconds)

    float3 eye_position_ = {0.0f, 1.5f, 0.0f};    //!< カメラのオフセット位置

    float rotate_v_ = 0.0f;    //!< 上下回転(ラジアン)
    float rotate_h_ = 0.0f;    //!< 左右回転(ラジアン)

    float recoil_pitch_  = 0.0f;    //view-only kick up, radians, goes back to 0(見た目だけの上への跳ね・ラジアン。0に戻る)
    float recoil_return_ = 9.0f;    //how fast recoil_pitch_ goes back(recoil_pitch_が戻る速さ)

    float mouse_sensitivity_ = DEFAULT_MOUSE_SENSITIVITY_;    //!< マウス感度
    float sensitivity_scale_ = 1.0f;                          //settings menu multiplier(設定メニューの倍率)

    float limit_pitch_min_ = -60.0f * DegToRad;    //!< 下限
    float limit_pitch_max_ = +89.0f * DegToRad;    //!< 上限

    float fov_ = 60.0f * DegToRad;    //!< 画角

    bool isZoom_ = false;    //!< ズーム中かどうか

    //show/hide the cursor and keep it in the middle while locked(ロック中はカーソルを隠して中央に置く)
    void ApplyCursor(bool lock_now);

    bool mouse_locked_  = false;    //the player's choice, toggled with MOUSE_LOCK_KEY(プレイヤーの選択。MOUSE_LOCK_KEYで切り替え)
    bool cursor_hidden_ = false;    //what we last told the system, so we only change it when needed(最後にシステムへ伝えた状態。必要な時だけ変える)
    s32  mouse_prev_x_  = 0;        //cursor position last frame, for the mouse movement(前フレームのカーソル位置。マウス移動量用)
    s32  mouse_prev_y_  = 0;

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
