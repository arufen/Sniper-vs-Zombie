//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
	float4	position_       : SV_Position;      //!< 座標       (スクリーン空間)
    float4  curr_position_  : CURR_POSITION;    //!< 現在の座標 (スクリーン空間)
	float3	world_position_ : WORLD_POSITION;   //!< ワールド座標
	float3	normal_         : NORMAL0;          //!< 法線
	float4	diffuse_        : COLOR0;           //!< Diffuseカラー
	float2	uv0_            : TEXCOORD0;        //!< テクスチャ座標
    float4  prev_position_  : PREV_POSITION;    //!< 1フレーム前の座標 (スクリーン空間) ※末尾に追加されているため注意
};

typedef	VS_OUTPUT_MODEL	PS_INPUT_MODEL;

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;

    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_); // surface normal

	// apply normal map
    N = Normalmap(N, input.world_position_, uv);

	// sample texture color
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);

    // combine texture with material/vertex diffuse color
    // (this way, materials with NO texture, like flat-color wood, still show their color instead of turning black/invisible)
    float4 baseColor = textureColor * input.diffuse_;   

	// alpha test (discard fully transparent pixels)
    if (baseColor.a < 0.5)
        discard;

	//------------------------------------------------------------
	// Basic lighting calculation (Lambert diffuse)
	//------------------------------------------------------------
    float3 light_dir = normalize(float3(-0.5, -1.0, -0.3)); // fake light direction (from upper-diagonal), just for testing
    float ndotl = max(dot(N, light_dir), 0.0); // how much this surface face the light (0 = no light, 1 = full light)
    float3 ambient = float3(0.3, 0.3, 0.3); // ambient light (minimum brightness even in shadow)
    float3 lighting = ambient + ndotl * float3(1.0, 1.0, 1.0); // combine ambient + directional light

    output.color0_ = float4(baseColor.rgb * lighting, baseColor.a);

    return output;
}