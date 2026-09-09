#include <Game/Component/ComponentFPSCameraController.h>
#include <System/Debug/DebugCamera.h>

void ComponentFPSCameraController::Init()
{
    __super::Init();

    scopeHandle = LoadGraph("data/Game/UI/scope.png");
}

void ComponentFPSCameraController::Update()
{
    if(DebugCamera::IsUse())
        return;

    __super::Update();

    auto owner = GetOwner();
    //----------------------------------------------------------
    // マウス視点操作
    //----------------------------------------------------------

    {
        // マウスカーソル座標(ウインドウのクライアント領域の座標)を取得
        s32 x, y;
        GetMousePoint(&x, &y);

        static s32 mouse_x = x;
        static s32 mouse_y = y;

        // 移動量を差分で計算
        s32 diff_x = x - mouse_x;
        s32 diff_y = y - mouse_y;

        if(IsMouseRepeat(MOUSE_INPUT_RIGHT)) {    // 右クリックしている間
            // 視点を回転させる
            rotate_h_ += static_cast<f32>(diff_x) * mouse_sensitivity_;
            rotate_v_ += static_cast<f32>(diff_y) * mouse_sensitivity_;

            // 上下可動範囲制限
            rotate_v_ = std::clamp(rotate_v_, limit_pitch_min_, limit_pitch_max_);

            // マウスカーソル位置を画面中央に戻す
            SetMousePoint(WINDOW_W / 2, WINDOW_H / 2);
        }

        // 値を次のフレームのために保存
        GetMousePoint(&mouse_x, &mouse_y);
    }

    //----------------------------------------------------------
    // FPS用のカメラを設定
    // 左右振り向き角度と上下角度で指定
    //----------------------------------------------------------

    // カメラのワールド行列
    matrix mat_camera = matrix::identity();
    {
        mat_camera = mul(mat_camera, matrix::rotateX(rotate_v_));
        mat_camera = mul(mat_camera, matrix::rotateY(rotate_h_));
        mat_camera = mul(mat_camera, matrix::translate(eye_position_ + owner->GetTranslate()));
    }
    owner->SetRotationAxisXYZ({rotate_v_ * RadToDeg, rotate_h_ * RadToDeg, 0.0f});

    // ビュー行列 = カメラのワールド行列の逆行列
    matrix mat_view = inverse(mat_camera);

    // ビュー行列を設定
    SetCameraViewMatrix(cast(mat_view));

    // 投影行列をパラメーターで設定
    SetupCamera_Perspective(fov_);
    SetCameraNearFar(0.01f, 1000.0f);

    //Zoomed when pressed E

    //Zoomed when pressed E
    {
        bool is_zooming = Input::IsKey(KEY_INPUT_E);
        isZoom_         = is_zooming;

        // move progress toward 1 (zoomed) or 0 (normal)
        float direction  = is_zooming ? 1.0f : -1.0f;
        zoom_progress_  += direction * (GetDeltaTime() / zoom_duration_);
        zoom_progress_   = std::clamp(zoom_progress_, 0.0f, 1.0f);

        // ease-out curve (fast start, slow finish)
        float t     = zoom_progress_;
        float eased = 1.0f - (1.0f - t) * (1.0f - t);

        // blend fov
        float normal_fov = 60.0f;
        float zoomed_fov = normal_fov * 0.33f;
        SetFov(normal_fov + (zoomed_fov - normal_fov) * eased);

        // blend mouse sensitivity
        float normal_sens = DEFAULT_MOUSE_SENSITIVITY_;
        float zoomed_sens = DEFAULT_MOUSE_SENSITIVITY_ * 0.25f;
        SetMouseSensitivity(normal_sens + (zoomed_sens - normal_sens) * eased);
    }
}

void ComponentFPSCameraController::LateDraw()
{
    __super::LateDraw();

    if(isZoom_) {
        DrawGraph(0, 0, scopeHandle, TRUE);
    }
}

float3 ComponentFPSCameraController::GetForwardVector(float offset) const
{
    matrix mat_rot = matrix::identity();
    mat_rot        = mul(mat_rot, matrix::rotateX(rotate_v_));
    mat_rot        = mul(mat_rot, matrix::rotateY(rotate_h_));

    float4 forward4 = mat_rot.axisVectorZ();
    float3 forward  = {forward4.x, forward4.y, forward4.z};

    return normalize(forward) * offset;
}

ComponentFPSCameraControllerPtr ComponentFPSCameraController::SetEyePosition(float3 pos)
{
    eye_position_ = pos;
    return std::dynamic_pointer_cast<ComponentFPSCameraController>(shared_from_this());
}

ComponentFPSCameraControllerPtr ComponentFPSCameraController::SetMouseSensitivity(float sensitivity)
{
    mouse_sensitivity_ = sensitivity;
    return std::dynamic_pointer_cast<ComponentFPSCameraController>(shared_from_this());
}

ComponentFPSCameraControllerPtr ComponentFPSCameraController::SetPitchLimit(float min_deg, float max_deg)
{
    limit_pitch_min_ = min_deg * DegToRad;
    limit_pitch_max_ = max_deg * DegToRad;
    return std::dynamic_pointer_cast<ComponentFPSCameraController>(shared_from_this());
}

ComponentFPSCameraControllerPtr ComponentFPSCameraController::SetFov(float fov_deg)
{
    fov_ = fov_deg * DegToRad;
    return std::dynamic_pointer_cast<ComponentFPSCameraController>(shared_from_this());
}

void ComponentFPSCameraController::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"FPS Camera Controller")) {
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::DragFloat(u8"マウス感度", &mouse_sensitivity_, 0.0001f);
            ImGui::DragFloat3(u8"目の位置オフセット", eye_position_.f32);
            ImGui::DragFloat(u8"上限(ラジアン)", &limit_pitch_max_);
            ImGui::DragFloat(u8"下限(ラジアン)", &limit_pitch_min_);
            ImGui::DragFloat(u8"画角", &fov_);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentFPSCameraController)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentFPSCameraController)
