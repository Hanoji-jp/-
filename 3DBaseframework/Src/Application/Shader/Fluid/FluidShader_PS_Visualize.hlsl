// ===================================================
// 流体シミュレーション 可視化パス
//  水のあるセルだけ水色で不透明に、その上に foam(泡)を重ねる。
//  コップ形状マスク(solid)の外＝壁は透明にして、背景（机など）を見せる。
//  水らしさは smoothstep(Lo,Hi) で塗り分け、飛沫の粒々を抑える。
// ===================================================

Texture2D<float4> g_quantity : register(t0);
Texture2D<float4> g_foam     : register(t1);
Texture2D<float4> g_solid    : register(t2);	// r: 1=内側 / 0=壁

cbuffer cbVisualize : register(b0)
{
	float4 g_waterColor;	// 水の色
	float4 g_spaceColor;	// 空(水なし)の色（未使用：空は透明にする）
	float4 g_foamColor;		// 泡の色
	float4 g_foamParams;	// x=泡ゲイン, y=水塗りLo, z=水塗りHi
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 coord = int2(svPos.xy);
	float4 quantity = g_quantity.Load(int3(coord, 0));
	float  foam     = g_foam.Load(int3(coord, 0)).r;
	float  solid    = g_solid.Load(int3(coord, 0)).r;	// 1=中 / 0=壁

	// z=質量 を Lo?Hi で 水らしさに
	float waterness = smoothstep(g_foamParams.y, g_foamParams.z, quantity.z);

	// 色は水色。泡を上から重ねる
	float3 color = g_waterColor.rgb;
	float foamness = saturate(foam * g_foamParams.x);
	color = lerp(color, g_foamColor.rgb, foamness);

	// 水／泡のある所だけ不透明。コップの外(壁)と空は透明。
	float alpha = saturate(max(waterness, foamness)) * solid;
	return float4(color, alpha);
}
