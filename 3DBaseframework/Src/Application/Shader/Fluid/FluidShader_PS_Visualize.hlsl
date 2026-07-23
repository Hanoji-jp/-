// ===================================================
// 流体シミュレーション 可視化パス
//  quantity(z=質量)を水らしさとして空色⇔水色を補間し、
//  その上に foam(泡量)を白い泡として重ねて表示する。
//  水らしさは smoothstep(Lo,Hi) で塗り分け、薄いセルも水として塗ると
//  飛沫の「気泡（粒々）」が減る。
// ===================================================

Texture2D<float4> g_quantity : register(t0);
Texture2D<float4> g_foam     : register(t1);

cbuffer cbVisualize : register(b0)
{
	float4 g_waterColor;	// 水の色
	float4 g_spaceColor;	// 空(水なし)の色
	float4 g_foamColor;		// 泡の色
	float4 g_foamParams;	// x=泡ゲイン, y=水塗りLo, z=水塗りHi
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 coord = int2(svPos.xy);
	float4 quantity = g_quantity.Load(int3(coord, 0));
	float  foam     = g_foam.Load(int3(coord, 0)).r;

	// z=質量 を Lo?Hi で 空色→水色 に塗り分ける
	float waterness = smoothstep(g_foamParams.y, g_foamParams.z, quantity.z);
	float3 color = lerp(g_spaceColor.rgb, g_waterColor.rgb, waterness);

	// 泡を上から重ねる
	float foamness = saturate(foam * g_foamParams.x);
	color = lerp(color, g_foamColor.rgb, foamness);

	return float4(color, 1.0f);
}
