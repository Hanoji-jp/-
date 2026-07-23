// ===================================================
// 流体シミュレーション 移流パス（ジオメトリシェーダ）
//  DirectX11 には WebGL の gl_PointSize が無いため、
//  1点を「中心±kSplatRadius の小クアッド」に展開して撒き込み範囲を作る。
//  （みずあそび.html の gl_PointSize=3 相当）
// ===================================================

cbuffer cbAdvection : register(b0)
{
	float2 g_gravity;	// 未使用（VSと共有の定数バッファ）
	float2 g_gridSize;	// グリッド解像度 (W, H)
};

static const float kSplatRadius = 1.5;	// PointSize=3 相当（中心±1.5px）

struct VSOutput
{
	float2 NewPos : TEXCOORD0;
	float4 NewQ   : TEXCOORD1;
};

struct GSOutput
{
	float4 Pos    : SV_Position;
	float2 NewPos : TEXCOORD0;	// 撒き込み中心（ピクセル座標）
	float4 NewQ   : TEXCOORD1;	// 運ぶ物理量
};

// ピクセル座標 → クリップ空間（row0=上 に合わせてY反転）
float2 PixelToClip(float2 px)
{
	return float2(px.x / g_gridSize.x * 2.0 - 1.0,
				  1.0 - px.y / g_gridSize.y * 2.0);
}

[maxvertexcount(4)]
void main(point VSOutput input[1], inout TriangleStream<GSOutput> stream)
{
	float2 center = input[0].NewPos;

	// トライアングルストリップ用に 左下・左上・右下・右上 の順で出力
	float2 corner[4] =
	{
		float2(-kSplatRadius, -kSplatRadius),
		float2(-kSplatRadius,  kSplatRadius),
		float2( kSplatRadius, -kSplatRadius),
		float2( kSplatRadius,  kSplatRadius),
	};

	GSOutput output;
	output.NewPos = center;
	output.NewQ = input[0].NewQ;

	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		output.Pos = float4(PixelToClip(center + corner[i]), 0.0, 1.0);
		stream.Append(output);
	}
}
