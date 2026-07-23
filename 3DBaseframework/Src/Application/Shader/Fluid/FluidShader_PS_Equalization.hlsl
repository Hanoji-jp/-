// ===================================================
// 流体 equalization（質量・体積の補填）
//  みずあそび.html の equalization-32-fragment を移植。
//  体積の余剰(w>1)は比率に応じて削り、不足(w<1)は基準比で補う。
//  水面を安定させ、数値誤差での破綻を防ぐ。
// ===================================================

Texture2D<float4> g_quantity : register(t0);

cbuffer cbEqualization : register(b0)
{
	float  g_surplusRate;	// 余剰を削る率
	float  g_deficitRate;	// 不足を補う率
	float2 _pad;
	float4 g_deficitRatio;	// 補填で加える比率 (0, 0, mass/volume, 1)
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));

	float4 surplusRatio = q / max(q.w, 1.0);
	q -= g_surplusRate * max(q.w - 1.0, 0.0) * surplusRatio;
	q += g_deficitRate * max(1.0 - q.w, 0.0) * g_deficitRatio;
	q.zw = max(q.zw, float2(0.0, 0.0));

	return q;
}
