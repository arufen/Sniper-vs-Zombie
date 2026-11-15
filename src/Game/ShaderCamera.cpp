//---------------------------------------------------------------------------
//!	@file	ShaderCamera.cpp
//! @brief	sends the real camera position to ps_model.fx every frame(毎フレーム、本当のカメラ位置をps_model.fxへ送る)
//---------------------------------------------------------------------------

#include "ShaderCamera.h"
#include "ObjectNames.h"

namespace SniperVsZombie {

namespace {
constexpr int SLOT_INFO =
    12;    //cbuffer slot b12 in ps_model.fx. b10 = engine camera, b11 = engine LightManager(ps_model.fxのb12。b10はエンジンのカメラ、b11はLightManager)
}

//! @brief 初期化
//! @return 初期化終了
bool ShaderCamera::Init()
{
    Super::Init();

    SetName(Names::SHADER_CAMERA);

    cb_info_ = CreateShaderConstantBuffer(sizeof(ShaderInfo));

    //Shadow timing is the first step of Scene::Draw: all cameras are already set, no model is drawn yet(ShadowタイミングはScene::Drawの最初: カメラは設定済み、モデルはまだ描かれていない)
    SetProc("SendCameraToShader", [this]() { SendCameraToShader(); }, ProcTiming::Shadow, ProcPriority::NORMAL);

    return true;
}

void ShaderCamera::SendCameraToShader()
{
    //camera position = translation of the inverse view matrix. works for ANY camera: FPS, debug, ComponentCamera(カメラ位置=ビュー行列の逆行列の移動成分。FPS・デバッグ・ComponentCameraのどれでも使える)
    matrix mat_view;
    mat_view                  = GetCameraViewMatrix();
    const matrix mat_camera   = inverse(mat_view);
    const float3 eye_position = mat_camera.translate();

    auto* info = reinterpret_cast<ShaderInfo*>(GetBufferShaderConstantBuffer(cb_info_));
    {
        info->eye_position_[0] = eye_position.x;
        info->eye_position_[1] = eye_position.y;
        info->eye_position_[2] = eye_position.z;
        info->enabled_         = 1.0f;
    }
    UpdateShaderConstantBuffer(cb_info_);
    SetShaderConstantBuffer(cb_info_, DX_SHADERTYPE_PIXEL, SLOT_INFO);
}

void ShaderCamera::Exit()
{
    //never leave a deleted buffer on the slot. the next scene would read stale numbers(消したバッファをスロットに残さない。次のシーンが古い数値を読んでしまう)
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, SLOT_INFO);

    if(cb_info_ != -1) {
        DeleteShaderConstantBuffer(cb_info_);
        cb_info_ = -1;
    }

    Super::Exit();
}
}    // namespace SniperVsZombie
