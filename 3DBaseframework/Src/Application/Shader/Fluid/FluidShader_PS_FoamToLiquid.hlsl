// ===================================================
// ’Y_F–A‚ªÁ‚¦‚Ä‰t‚Ö–ß‚éi‰t‚Ì¿—Ê‚ğ‘‚â‚·j
//  Œ¸Š‚ÅÁ‚¦‚é–A‚Ô‚ñ foam*(1-decay) ‚ğA‚»‚ÌƒZƒ‹‚Ì‰t¿—Ê(z)‚Ö–ß‚·B
//  –A‚Í‰t–Ê•t‹ß‚É‹‚é‚Ì‚ÅA–ß‚Á‚½‰t‚ª…ˆÊ‚ğ‰Ÿ‚µã‚°‚éB
// ===================================================

Texture2D<float4> g_quantity : register(t0);
Texture2D<float4> g_foam     : register(t1);

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
	float4 q = g_quantity.Load(int3(c, 0));
	float  F = g_foam.Load(int3(c, 0)).r;

	float returned = F * (1.0 - g_decayRate);
	q.z += returned;
	q.w = max(q.w, q.z);	// ‘ÌÏ‚Í¿—ÊˆÈã‚ğŠm•Ûi‰t‚ª‘‚¦‚½‚Ô‚ñ‚ÌêŠj
	return q;
}
