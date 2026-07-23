// ===================================================
// 流体 圧力：pressure-gradient（圧力勾配→connection）
//  みずあそび.html の pressure-gradient-fragment を移植。
//  4近傍への (圧力差)/(質量和) を connection として出力する。
//   connection = (outPressure - inPressure) / max(inMass + outMass, minMass)
// ===================================================

Texture2D<float4> g_quantity  : register(t0);
Texture2D<float4> g_intensity : register(t1);

static const float kMinMass = 0.001;

float4 main(float4 svPos : SV_Position) : SV_Target
{
	uint W, H;
	g_quantity.GetDimensions(W, H);
	int2 maxCoord = int2(W - 1, H - 1);

	int2 c  = int2(svPos.xy);
	int2 pX = max(c - int2(1, 0), int2(0, 0));
	int2 pY = max(c - int2(0, 1), int2(0, 0));
	int2 nX = min(c + int2(1, 0), maxCoord);
	int2 nY = min(c + int2(0, 1), maxCoord);

	float4 q   = g_quantity.Load(int3(c,  0));
	float4 qpx = g_quantity.Load(int3(pX, 0));
	float4 qpy = g_quantity.Load(int3(pY, 0));
	float4 qnx = g_quantity.Load(int3(nX, 0));
	float4 qny = g_quantity.Load(int3(nY, 0));

	float4 it  = g_intensity.Load(int3(c,  0));
	float4 ipx = g_intensity.Load(int3(pX, 0));
	float4 ipy = g_intensity.Load(int3(pY, 0));
	float4 inx = g_intensity.Load(int3(nX, 0));
	float4 iny = g_intensity.Load(int3(nY, 0));

	float  inMass  = q.z;
	float4 outMass = float4(qpx.z, qpy.z, qnx.z, qny.z);
	float  inPressure  = it.w;
	float4 outPressure = float4(ipx.w, ipy.w, inx.w, iny.w);

	return (outPressure - inPressure) / max(inMass + outMass, kMinMass);
}
