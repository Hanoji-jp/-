// ===================================================
// 炭酸：泡の発生（液面の乱流から泡を作る）
//  「上が空気＝液面」で、動いている液セルほど泡が立つ。
//  発生ぶんの質量は液から預かる（別パス FoamLoss で減算）。総質量は保存。
//  出力：foam.r（泡量）を増やしたもの。
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

static const float kMinMass = 0.001;

// このセルで液→泡へ移す量：液面(上が空気)で乱流があるほど泡が立つ。質量を超えない。
float ComputeGen(int2 c, float4 q)
{
	float speed = length(q.xy) / max(q.z, kMinMass);
	float turbulence = saturate(speed - g_speedThresh);
	float isLiquid = step(g_massThresh, q.z);
	float massAbove = g_quantity.Load(int3(c - int2(0, 1), 0)).z;
	float isSurface = step(massAbove, g_massThresh);	// 上が空気＝液面
	return min(g_genRate * turbulence * isLiquid * isSurface, q.z);
}

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));
	float4 f = g_foam.Load(int3(c, 0));

	f.r += ComputeGen(c, q);
	return f;
}
