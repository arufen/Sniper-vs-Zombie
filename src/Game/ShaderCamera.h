//---------------------------------------------------------------------------
//!	@file	ShaderCamera.h
//! @brief	sends the real camera position to ps_model.fx every frame(毎フレーム、本当のカメラ位置をps_model.fxへ送る)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>

namespace SniperVsZombie {
USING_PTR(ShaderCamera);

//put ONE of these in every scene that draws 3D models(3Dモデルを描画するシーンに1つだけ置く)
//why: ps_model.fx needs the camera position (rim light, fog). the engine only sends it from ComponentCamera,(理由: ps_model.fxはカメラ位置が必要。エンジンはComponentCameraからしか送らない)
//but the FPS player camera sets the DxLib camera directly, so the shader got an OLD position. see guide 06(FPSプレイヤーカメラはDxLibのカメラを直接設定するので、シェーダーには古い位置が届いていた)
class ShaderCamera : public Object
{
public:
    BP_OBJECT_DECL(ShaderCamera, "SniperVsZombie::ShaderCamera");

    bool Init() override;

    void Exit() override;

private:
    //runs at the start of Scene::Draw, before any model is drawn(Scene::Drawの最初、モデル描画より前に実行)
    void SendCameraToShader();

    //what ps_model.fx reads at b12. the order and sizes must match the cbuffer in the shader(ps_model.fxがb12で読む内容。並びとサイズをシェーダーのcbufferと一致させる)
    struct ShaderInfo
    {
        float eye_position_[3];    //camera position that is really used for drawing(実際に描画で使われるカメラ位置)
        float enabled_;            //1 = this buffer is valid(1=このバッファは有効)
    };

    int cb_info_ = -1;    //[DxLib] constant buffer handle(定数バッファハンドル)
};
}    // namespace SniperVsZombie
