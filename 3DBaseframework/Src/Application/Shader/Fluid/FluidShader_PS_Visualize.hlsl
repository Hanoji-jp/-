// ===================================================
// 流体シミュレーション 可視化パス
//  quantity(z=質量)を 0～1 の「水らしさ」とみなし、空色と水色を補間して
//  表示用テクスチャ(RGBA8)へ書き込む。コップ内側の板ポリゴンへ貼って表示する。
//  Phase1の見た目確認用。以降のフェーズでは法線・反射を用いた本描画へ差し替える。
// ===================================================

Texture2D<float4> g_quantity : register(t0);

cbuffer cbVisualize : register(b0)
{
	float4 g_waterColor;	// 水の色
	float4 g_spaceColor;	// 空(水なし)の色
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 coord = int2(svPos.xy);
	float4 quantity = g_quantity.Load(int3(coord, 0));

	// z=質量 を水らしさとして扱う
	float waterness = saturate(quantity.z);

	float3 color = lerp(g_spaceColor.rgb, g_waterColor.rgb, waterness);
	return float4(color, 1.0f);
}
