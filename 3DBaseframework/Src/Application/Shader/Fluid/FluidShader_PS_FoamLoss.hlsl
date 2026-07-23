// ===================================================
// ’Y_F–A‚Ì”­¶‚É”º‚¤‰t‚Ì¿—ÊŒ¸ZiFoamGen ‚Æ‘Îj
//  FoamGen ‚Æ“¯‚¶—Ê‚ğ‰t‚Ì¿—Ê(z)‚©‚çˆø‚­B‘¿—Ê‚Í•Û‘¶‚³‚ê‚éB
// ===================================================

Texture2D<float4> g_quantity : register(t0);

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

static const float kMinMass = 0.001;

// FoamGen ‚Æ“¯‚¶®i‰t–Ê‚Ì——¬‚Ô‚ñj
float ComputeGen(int2 c, float4 q)
{
	float speed = length(q.xy) / max(q.z, kMinMass);
	float turbulence = saturate(speed - g_speedThresh);
	float isLiquid = step(g_massThresh, q.z);
	float massAbove = g_quantity.Load(int3(c - int2(0, 1), 0)).z;
	float isSurface = step(massAbove, g_massThresh);
	return min(g_genRate * turbulence * isLiquid * isSurface, q.z);
}

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));

	q.z = max(q.z - ComputeGen(c, q), 0.0);
	return q;
}
