//---------------------------------------------------------------------------
//!	@file	CameraProbe.cpp
//! @brief	dev tool: shows the camera position on screen and copies it to the clipboard(開発ツール: カメラ位置を画面に表示し、クリップボードにコピーする)
//---------------------------------------------------------------------------

#include "CameraProbe.h"
#include "ObjectNames.h"
#include <Game/Component/ComponentFPSCameraController.h>
#include <System/Debug/DebugCamera.h>

namespace SniperVsZombie {

namespace {
constexpr float TARGET_DISTANCE  = 100.0f;    //how far in front of the eye the target point is(目からどれだけ前を注視点にするか)
constexpr float COPIED_SHOW_TIME = 1.5f;      //seconds "copied!" stays on screen(「コピーしました」の表示秒数)
}

//! @brief 初期化
//! @return 初期化終了
bool CameraProbe::Init()
{
    Super::Init();

    SetName(Names::CAMERA_PROBE);

    return true;
}

void CameraProbe::Update()
{
    Super::Update();

    if(IsKeyDown(TOGGLE_KEY))
        visible_ = !visible_;

    ReadCamera();

    if(IsKeyDown(COPY_KEY)) {
        SetClipboardText(MakeCopyText().c_str());
        copied_timer_ = COPIED_SHOW_TIME;
    }

    if(copied_timer_ > 0.0f)
        copied_timer_ -= GetDeltaTime();
}

void CameraProbe::ReadCamera()
{
    //same trick as ShaderCamera: camera world matrix = inverse of the view matrix(ShaderCameraと同じ方法: カメラのワールド行列=ビュー行列の逆行列)
    //works for every camera, even the FPS one that sets the view matrix directly(ビュー行列を直接設定するFPSカメラでも使える)
    matrix mat_view;
    mat_view                = GetCameraViewMatrix();
    const matrix mat_camera = inverse(mat_view);

    eye_    = mat_camera.translate();
    target_ = eye_ + normalize(mat_camera.axisZ()) * TARGET_DISTANCE;

    //FPS look angles, only when the player camera exists (MainStage)(FPSの視線角度。プレイヤーカメラがある時だけ(MainStage))
    has_fps_ = false;
    if(auto camera = Scene::Object::Get<Object>(Names::CAMERA)) {
        if(auto controller = camera->GetComponent<ComponentFPSCameraController>()) {
            has_fps_    = true;
            fps_angles_ = controller->GetLookAngles();
        }
    }
}

std::string CameraProbe::MakeCopyText() const
{
    char text[256];
    snprintf(text,
             sizeof(text),
             "eye = {%.3ff, %.3ff, %.3ff}; target = {%.3ff, %.3ff, %.3ff};",
             (float)eye_.x,
             (float)eye_.y,
             (float)eye_.z,
             (float)target_.x,
             (float)target_.y,
             (float)target_.z);

    std::string result = text;

    //the FPS angles are only useful when NOT in the debug camera (P0 = player start look)(FPS角度はデバッグカメラでない時だけ意味がある。P0=開始時の視線)
    if(has_fps_ && !DebugCamera::IsUse()) {
        snprintf(text, sizeof(text), " look = {%.2ff, %.2ff};", (float)fps_angles_.x, (float)fps_angles_.y);
        result += text;
    }
    return result;
}

void CameraProbe::PostDraw()
{
    Super::PostDraw();

    if(!visible_)
        return;

    //small black box so the numbers are readable on any background(どんな背景でも読めるよう小さな黒い箱)
    const int x = 10;
    const int y = WINDOW_H - 110;
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);
    DrawBox(x - 5, y - 5, x + 620, y + 95, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    const unsigned int white  = GetColor(255, 255, 255);
    const unsigned int yellow = GetColor(255, 220, 80);

    const int old_font_size = GetFontSize();    //put it back after, other text uses the global size too(後で戻す。他の文字もグローバルサイズを使う)
    SetFontSize(16);
    DrawFormatString(x, y, yellow, "CAMERA PROBE  [F4] debug cam %s  [F7] hide  [F8] copy", DebugCamera::IsUse() ? "ON" : "OFF");
    DrawFormatString(x, y + 20, white, "eye    = {%.3f, %.3f, %.3f}", (float)eye_.x, (float)eye_.y, (float)eye_.z);
    DrawFormatString(x, y + 40, white, "target = {%.3f, %.3f, %.3f}", (float)target_.x, (float)target_.y, (float)target_.z);
    if(has_fps_)
        DrawFormatString(x, y + 60, white, "FPS look (yaw, pitch) = {%.2f, %.2f}", (float)fps_angles_.x, (float)fps_angles_.y);
    if(copied_timer_ > 0.0f)
        DrawFormatString(x + 420, y + 60, GetColor(80, 255, 80), "COPIED!");
    SetFontSize(old_font_size);
}

void CreateCameraProbe()
{
#ifdef _DEBUG
    Scene::Object::Create<CameraProbe>();
#endif
}

}    // namespace SniperVsZombie
