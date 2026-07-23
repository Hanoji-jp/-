// ===================================================
// 流体 圧力：solution（圧力のヤコビ反復）
//  みずあそび.html の solution-fragment を移植。
//  newPressure = (source + Σ weight_i * neighborPressure_i) / Σ weight_i
//  xyz(重み・source)は保持し、w(圧力)のみ更新する。
// ===================================================

Texture2D<float4> g_intensity : register(t0);

static const float kMinWeight = 0.001;

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 it = g_intensity.Load(int3(c, 0));
	float4 pX = g_intensity.Load(int3(c - int2(1, 0), 0));
	float4 pY = g_intensity.Load(int3(c - int2(0, 1), 0));
	float4 nX = g_intensity.Load(int3(c + int2(1, 0), 0));
	float4 nY = g_intensity.Load(int3(c + int2(0, 1), 0));

	float4 weight   = float4(it.x, it.y, nX.x, nY.y);
	float4 pressure = float4(pX.w, pY.w, nX.w, nY.w);

	float sumWeight = max(dot(float4(1, 1, 1, 1), weight), kMinWeight);
	float newPressure = (it.z + dot(pressure, weight)) / sumWeight;

	return float4(it.xyz, newPressure);
}
