//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// this model has a normal map texture only if SHADER_VARIANT says so
// (same rule as dxlib_vs_model.h, so vertex shader and pixel shader agree)
#define DX_MV1_VERTEX_TYPE_NMAP_1FRAME (4)
#if (SHADER_VARIANT >= DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
#define USE_NORMALMAP
#endif

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

	// apply normal map (only if this model actually has one)
#if defined(USE_NORMALMAP)
    N = Normalmap(N, input.world_position_, uv);
#endif

	// sample texture color
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);

    // combine texture with material/vertex diffuse color
    // (this way, materials with NO texture, like flat-color wood, still show their color instead of turning black/invisible)
    float4 baseColor = textureColor * input.diffuse_;

	// alpha test (discard fully transparent pixels)
    if (baseColor.a < 0.5)
        discard;

	//------------------------------------------------------------
	// 3-point lighting (key + fill + rim), fully self-contained
	//------------------------------------------------------------
    float3 key_dir  = normalize(float3(-0.5, -1.0, -0.3));  // main light, from upper-diagonal
    float3 fill_dir = normalize(float3( 0.6, -0.2,  0.5));  // soft fill light, opposite-ish side, fills in shadows
    float3 rim_dir  = normalize(float3( 0.0,  0.4, -1.0));  // rim light, from behind, adds edge highlight

    float key_ndotl  = max(dot(N, key_dir),  0.0);
    float fill_ndotl = max(dot(N, fill_dir), 0.0);
    float rim_ndotl  = max(dot(N, rim_dir),  0.0);

    float3 key_color  = float3(1.0, 1.0, 1.0)  * key_ndotl  * 1.0;  // main light strength
    float3 fill_color = float3(0.6, 0.7, 0.9)  * fill_ndotl * 0.4;  // fill light, slightly blue, weaker
    float3 rim_color  = float3(1.0, 0.95, 0.8) * rim_ndotl  * 0.3;  // rim light, slightly warm, weaker

    float3 ambient = float3(0.35, 0.35, 0.4); // ambient light (minimum brightness even in full shadow)
    float3 lighting = ambient + key_color + fill_color + rim_color;

    output.color0_ = float4(baseColor.rgb * lighting, baseColor.a);

    return output;
}