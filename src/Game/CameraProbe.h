//---------------------------------------------------------------------------
//!	@file	CameraProbe.h
//! @brief	dev tool: shows the camera position on screen and copies it to the clipboard(開発ツール: カメラ位置を画面に表示し、クリップボードにコピーする)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(CameraProbe);

//how to use: F4 = fly with the debug camera, F7 = show/hide numbers, F8 = copy them(使い方: F4=デバッグカメラで飛ぶ、F7=数値の表示/非表示、F8=コピー)
//then paste the numbers to the AI or into StageConfig.h(その後、数値をAIやStageConfig.hに貼る)
//only created in Debug builds (see CreateCameraProbe)(Debugビルドでのみ作られる。CreateCameraProbe参照)
class CameraProbe : public Object
{
public:
    BP_OBJECT_DECL(CameraProbe, "SniperVsZombie::CameraProbe");

    bool Init() override;

    void Update() override;

    void PostDraw() override;

    static constexpr int TOGGLE_KEY = KEY_INPUT_F7;    //F6 is taken by the engine (hide mouse)(F6はエンジンが使用中(マウス非表示))
    static constexpr int COPY_KEY   = KEY_INPUT_F8;

private:
    //read the camera that is REALLY used for drawing (FPS, debug or ComponentCamera)(実際に描画に使われているカメラを読む。FPS・デバッグ・ComponentCamera)
    void ReadCamera();

    //build the text that F8 copies(F8でコピーする文字列を作る)
    std::string MakeCopyText() const;

    bool   visible_      = true;    //overlay on/off(表示のオン/オフ)
    float3 eye_          = {0.0f, 0.0f, 0.0f};
    float3 target_       = {0.0f, 0.0f, 0.0f};    //a point 100 units in front of the eye(目の100ユニット前の点)
    bool   has_fps_      = false;                 //true = the FPS player camera exists in this scene(true=このシーンにFPSカメラがある)
    float2 fps_angles_   = float2(0.0f, 0.0f);    //FPS yaw/pitch in degrees(FPSカメラの左右/上下角度・度)
    float  copied_timer_ = 0.0f;                  //shows "copied!" for a moment(「コピーしました」を少し表示する)
};

//call in a scene's Init. does nothing in Release builds(シーンのInitで呼ぶ。Releaseビルドでは何もしない)
void CreateCameraProbe();

}    // namespace SniperVsZombie
