// ===================================================
// ’Y_F–A‚ÌŒ¸ŠiFoamToLiquid ‚Æ‘Îj
//  –A‚ğ decay ”{‚É‚·‚éic‚è‚Í FoamToLiquid ‚Å‰t‚Ö–ß‚µ‚Ä‚¢‚éjB
// ===================================================

Texture2D<float4> g_foam : register(t0);

cbuffer cbFoam : register(b0)
{
	float g_genRate;
	float g_speedThresh;
	float g_massThresh;
	float g_riseRate;
	float g_spreadRate;
	float g_decayRate;
	float2 _pad;
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float F = g_foam.Load(int3(c, 0)).r;
	return float4(F * g_decayRate, 0.0, 0.0, 0.0);
}
