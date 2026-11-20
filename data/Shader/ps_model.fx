//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー - moonlit low-poly toon night(月夜のローポリ・トゥーン)
//!	@note	EVERY MV1 model uses this file: city, grass, sky, zombies, gun(全MV1モデルがこのファイルを使う)
//!	@note	hot reload: save while the game runs and it recompiles(ゲーム実行中に保存すると再コンパイルされる)
//!	@note	guide: AI Context/AdviceFromAI/Shader/(解説はこちら)
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// this model has a normal map texture only if SHADER_VARIANT says so
// (same rule as dxlib_vs_model.h, so vertex shader and pixel shader agree)
#define DX_MV1_VERTEX_TYPE_NMAP_1FRAME (4)
#if (SHADER_VARIANT >= DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
#define USE_NORMALMAP
#endif

//============================================================================
// TUNING - change these while the game runs(調整値 - ゲーム実行中に変更できる)
//============================================================================
// DEBUG: 0 = normal game look, 1 = show normals as color, 2 = show moon bands only, 3 = show which normals got flipped(0=通常、1=法線を色で表示、2=月の段階だけ、3=反転された法線を表示)
// in mode 1: roofs/ground should be GREEN (normal points up)(モード1: 屋根と地面は緑になるはず。法線が上向き)
// in mode 3: RED = this pixel's normal was pointing the wrong way and got fixed(モード3: 赤=法線が逆向きだったので直した)
static const int DEBUG_VIEW = 0;

// STEP 0: fix normals that point INTO the model. see guide 07(モデルの内側を向いた法線を直す。解説07)
static const bool FIX_NORMALS = true;  //false = use the model normals as they are, to compare(false=比較用にモデルの法線をそのまま使う)

// STEP 2: moon(月)
static const float3 MOON_DIR      = normalize(float3(-0.5, -0.5, 0.5));  //direction the moonlight TRAVELS. y must be NEGATIVE = light comes down(月光が進む向き。yは必ず負=光が上から来る)
static const float3 MOON_COLOR    = float3(0.85, 0.90, 1.0);            //almost white, a little cold(ほぼ白、少し冷たい)
static const float  MOON_STRENGTH = 2.0;                                //(月の強さ)

// STEP 2: hemisphere ambient(半球アンビエント)
static const float3 SKY_AMBIENT    = float3(0.18, 0.20, 0.27);  //light on surfaces facing UP(上向きの面の光)
static const float3 GROUND_AMBIENT = float3(0.06, 0.06, 0.08);  //light on surfaces facing DOWN(下向きの面の光)

// STEP 3: toon bands(トゥーンの段階)
static const float BANDS = 3.0;  //how many light steps. 2 = hard, 5 = softer(光の段階数)

// STEP 4: rim light(リムライト)
static const float3 RIM_COLOR    = float3(0.55, 0.70, 1.0);  //(リムの色)
static const float  RIM_STRENGTH = 0.3;                      //(リムの強さ)
static const float  RIM_START    = 0.65;                     //rim begins at this edge-ness, higher = thinner(この値から縁が光る。高いほど細い)
static const float  RIM_END      = 0.90;                     //rim is full at this edge-ness(この値で最大)

// STEP 5: fog(フォグ)
static const float3 FOG_COLOR   = float3(0.04, 0.06, 0.12);  //dark navy night(濃紺の夜)
static const float  FOG_START   = 300.0;                     //no fog closer than this(これより近いとフォグなし)
static const float  FOG_DENSITY = 0.0008;                    //bigger = thicker(大きいほど濃い)

//============================================================================
// camera info from ComponentCamera, slot b10. FALLBACK ONLY(ComponentCameraのカメラ情報、b10。予備のみ)
// the FPS player camera never fills this, so it can hold an OLD camera. see guide 06(FPSプレイヤーカメラはこれを更新しないので、古いカメラが残ることがある)
//============================================================================
cbuffer CameraInfo : register(b10)
{
    float4x4 camera_view_;          //view matrix, unused here(ビュー行列。ここでは未使用)
    float4x4 camera_proj_;          //projection matrix, unused here(投影行列。ここでは未使用)
    float3   camera_eye_position_;  //camera position in the world(ワールドでのカメラ位置)
};

//============================================================================
// the camera that is REALLY drawing, from src/Game/ShaderCamera.cpp, slot b12, every frame(本当に描画しているカメラ。ShaderCamera.cppから毎フレームb12に届く)
// the layout must match ShaderCamera::ShaderInfo in C++(C++側のShaderInfoと並びを一致させる)
//============================================================================
cbuffer ShaderCameraInfo : register(b12)
{
    float3 real_eye_position_;  //camera position in the world(ワールドでのカメラ位置)
    float  real_eye_enabled_;   //1 = a ShaderCamera object exists. an empty slot reads as 0(1=ShaderCameraがある。空のスロットは0として読まれる)
};

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
// メイン関数 - runs once for EVERY pixel of every model(全モデルの全ピクセルで1回ずつ実行)
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;

    float2 uv = input.uv0_;
    float3 N  = normalize(input.normal_);  //which way this pixel's surface faces, from the model file(このピクセルの面の向き。モデルファイルの値)

    // camera position: from ShaderCamera (b12) when it exists, otherwise the old b10(カメラ位置: ShaderCamera(b12)があればそれ、なければ古いb10)
    float3 eye = (real_eye_enabled_ > 0.5) ? real_eye_position_ : camera_eye_position_;

    //------------------------------------------------------------
    // STEP 0: normal fix - the REAL direction of this triangle, measured on screen(法線の修正 - 画面上で測った、この三角形の本当の向き)
    // must run BEFORE discard: ddx/ddy read the neighbour pixels(discardより前に実行: ddx/ddyは隣のピクセルを読む)
    //------------------------------------------------------------
    float3 dp_dx  = ddx(input.world_position_);           //world position change to the pixel on the RIGHT(右隣のピクセルとのワールド座標の差)
    float3 dp_dy  = ddy(input.world_position_);           //world position change to the pixel BELOW(下隣のピクセルとのワールド座標の差)
    float3 face_N = normalize(cross(dp_dy, dp_dx));       //2 arrows lying ON the triangle -> cross = arrow sticking OUT of it(三角形の上の2本の矢印 → 外積=面から突き出る矢印)
    face_N       *= (dot(face_N, eye - input.world_position_) < 0.0) ? -1.0 : 1.0;  //we can see this side, so it must face the camera(この面が見えている=カメラを向いているはず)

    bool flipped = FIX_NORMALS && dot(N, face_N) < 0.0;  //model normal points the other way = wrong(モデルの法線が逆向き=間違い)
    if (flipped)
        N = -N;                                           //flip only the wrong ones: smooth shading stays(間違いだけ反転。なめらかな陰影は残る)

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
    // STEP 1: light direction - dot() needs the way TO the light(ライトの向き - dotには「ライトへ向かう向き」が必要)
    //------------------------------------------------------------
    float3 L     = -MOON_DIR;                  //flip: from surface toward the moon(反転: 面から月へ)
    float  ndotl = saturate(dot(N, L));        //1 = faces the moon, 0 = faces away(1=月を向く、0=背を向ける)

    //------------------------------------------------------------
    // STEP 3: toon bands - cut the smooth 0..1 into hard steps(トゥーン - なめらかな0〜1を段階に切る)
    //------------------------------------------------------------
    float band = saturate(floor(ndotl * BANDS) / (BANDS - 1.0));

    //------------------------------------------------------------
    // STEP 2: moonlight + hemisphere ambient(月光 + 半球アンビエント)
    //------------------------------------------------------------
    float3 moon    = MOON_COLOR * MOON_STRENGTH * band;
    float  up      = N.y * 0.5 + 0.5;                                 //-1..1 -> 0..1, 1 = faces straight up(1=真上を向く)
    float3 ambient = lerp(GROUND_AMBIENT, SKY_AMBIENT, up);

    float3 color = baseColor.rgb * (ambient + moon);

    //------------------------------------------------------------
    // STEP 4: rim light - glow where the surface turns away from the camera(リム - カメラから見て面が横を向く所が光る)
    //------------------------------------------------------------
    float3 V     = normalize(eye - input.world_position_);                   //from pixel toward the camera(ピクセルからカメラへ)
    float  edge  = 1.0 - saturate(dot(N, V));                                //0 = faces camera, 1 = side-on(0=正面、1=真横)
    float  rim   = smoothstep(RIM_START, RIM_END, edge);
    rim         *= 0.4 + 0.6 * band;                                         //weaker in shadow, so dark walls do not glow(影では弱く。暗い壁が光らないように)
    color       += RIM_COLOR * RIM_STRENGTH * rim;

    //------------------------------------------------------------
    // STEP 5: distance fog - far pixels fade into the night color(距離フォグ - 遠いピクセルは夜の色に溶ける)
    //------------------------------------------------------------
    float dist = distance(input.world_position_, eye);
    float fog  = 1.0 - exp(-max(dist - FOG_START, 0.0) * FOG_DENSITY);  //0 = clear, 1 = full fog(0=くっきり、1=完全にフォグ)
    color      = lerp(color, FOG_COLOR, fog);

    output.color0_ = float4(color, baseColor.a);

    //------------------------------------------------------------
    // DEBUG views - replace the final color to SEE a value(デバッグ表示 - 最終色を置き換えて値を「見る」)
    //------------------------------------------------------------
    if (DEBUG_VIEW == 1)
        output.color0_ = float4(N * 0.5 + 0.5, 1.0);  //x->red, y->green, z->blue(x→赤、y→緑、z→青)
    else if (DEBUG_VIEW == 2)
        output.color0_ = float4(band.xxx, 1.0);       //black = shadow band, white = lit band(黒=影、白=光)
    else if (DEBUG_VIEW == 3)
        output.color0_ = flipped ? float4(1.0, 0.1, 0.1, 1.0) : float4(0.2, 0.2, 0.2, 1.0);  //red = fixed, grey = was already correct(赤=修正済み、灰=元から正しい)

    return output;
}
