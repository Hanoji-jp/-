// ===================================================
// 流体シミュレーション 移流パス（頂点シェーダ）
//  みずあそび.html の advection-32-vertex を移植。
//  1セル=1点。セルの物理量から速度を求め、重力を加えて移動先を計算し、
//  「運ぶ物理量(NewQ)」と「移動先ピクセル座標(NewPos)」をジオメトリシェーダへ渡す。
//  さらに、コップ形状マスク(solid)の壁セルへは移動しない（＝グラスの当たり判定）。
//
//  ★放置時の「左右の盛り上がり」対策：
//   ・速度に減衰(g_damp<1)を掛けて、注水停止後のスロッシュ（横揺れ）を収める。
//   ・壁へ入り込むときの滑りは「下方向のみ」許可し、壁を伝って上へ登らせない。
// ===================================================

Texture2D<float4> g_quantity : register(t0);
Texture2D<float4> g_solid    : register(t1);	// r: 1=内側 / 0=壁

cbuffer cbAdvection : register(b0)
{
	float2 g_gravity;	// 重力（row増加＝画面下が +）
	float2 g_gridSize;	// グリッド解像度 (W, H)
	float  g_damp;		// 速度の減衰（1未満で沈静化。重力には掛けない）
	float3 g_pad;
};

static const float kMinMass = 0.001;

struct VSOutput
{
	float2 NewPos : TEXCOORD0;	// 移動先のピクセル座標
	float4 NewQ   : TEXCOORD1;	// 運ぶ物理量 (xy=運動量, z=質量, w=体積)
};

// そのピクセル座標のセルが壁(solid=0)かどうか
bool IsSolid(float2 pixel)
{
	int2 c = int2(clamp(pixel, float2(0.5, 0.5), g_gridSize - 0.5));
	return g_solid.Load(int3(c, 0)).r < 0.5;
}

VSOutput main(float2 position : POSITION)
{
	int2 cell = int2(position);					// (col+0.5, row+0.5) → (col, row)
	float4 q = g_quantity.Load(int3(cell, 0));

	// 速度は減衰させる（重力は別で加える＝落下は保つ）。放置時の横揺れを収める。
	float2 velocity = (q.xy / max(q.z, kMinMass)) * g_damp;
	float2 target = clamp(position + velocity + g_gravity, float2(0.5, 0.5), g_gridSize - 0.5);

	// 壁へ入り込むなら滑らせる。ただし上方向へは登らせない（壁際の盛り上がり防止）。
	//  ・slideX：水平に滑る（常に許可）
	//  ・slideY：垂直に滑るが、下向き(target.y >= position.y)のときだけ許可
	float2 newPos = position;
	if (!IsSolid(target)) { newPos = target; }
	else
	{
		float2 slideX = float2(target.x, position.y);
		float2 slideY = float2(position.x, target.y);
		if      (!IsSolid(slideX)) { newPos = slideX; }
		else if (!IsSolid(slideY) && target.y >= position.y) { newPos = slideY; }
	}

	VSOutput output;
	output.NewPos = newPos;
	// 運動量 = 質量 × 変位、質量・体積はそのまま運ぶ
	output.NewQ = float4(q.z * (newPos - position), q.z, q.w);
	return output;
}
