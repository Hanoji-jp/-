// ===================================================
// 流体シミュレーション 移流パス（頂点シェーダ）
//  みずあそび.html の advection-32-vertex を移植。
//  1セル=1点。セルの物理量から速度を求め、重力を加えて移動先を計算し、
//  「運ぶ物理量(NewQ)」と「移動先ピクセル座標(NewPos)」をジオメトリシェーダへ渡す。
//  実際のグリッドへの撒き込み(splat)はGS→PSと加算ブレンドで行う。
// ===================================================

Texture2D<float4> g_quantity : register(t0);

cbuffer cbAdvection : register(b0)
{
	float2 g_gravity;	// 重力（row増加方向＝画面下が +）
	float2 g_gridSize;	// グリッド解像度 (W, H)
};

static const float kMinMass = 0.001;

struct VSOutput
{
	float2 NewPos : TEXCOORD0;	// 移動後のピクセル座標
	float4 NewQ   : TEXCOORD1;	// 運ぶ物理量 (xy=運動量, z=質量, w=体積)
};

VSOutput main(float2 position : POSITION)
{
	int2 cell = int2(position);					// (col+0.5, row+0.5) → (col, row)
	float4 q = g_quantity.Load(int3(cell, 0));

	float2 velocity = q.xy / max(q.z, kMinMass);
	float2 newPos = clamp(position + velocity + g_gravity, float2(0.5, 0.5), g_gridSize - 0.5);

	VSOutput output;
	output.NewPos = newPos;
	// 運動量 = 質量 × 変位、質量・体積はそのまま運ぶ
	output.NewQ = float4(q.z * (newPos - position), q.z, q.w);
	return output;
}
