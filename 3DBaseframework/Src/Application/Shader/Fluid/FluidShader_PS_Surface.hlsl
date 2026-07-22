// ===================================================
// 流体シミュレーション 表面（自由表面／体積フラックス）パス
//  みずあそび.html の surface-32 を移植。
//  各セルと4近傍の 質量(z)・体積(w) から、体積の流束を求めて w を更新する。
//  （運動量・質量は変えず、体積のみ再配分して自由表面を作る）
// ===================================================

Texture2D<float4> g_quantity : register(t0);

static const float kSurfaceFlux = 0.24;	// 体積流束の上限係数（32bit版）

float4 main(float4 svPos : SV_Position) : SV_Target
{
	uint width, height;
	g_quantity.GetDimensions(width, height);
	int2 maxCoord = int2(width - 1, height - 1);

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

	float  inMass  = q.z;
	float4 outMass = float4(qpx.z, qpy.z, qnx.z, qny.z);
	float  inVol   = q.w;
	float4 outVol  = float4(qpx.w, qpy.w, qnx.w, qny.w);

	float  inSpace  = saturate(inVol - inMass);
	float4 outSpace = saturate(outVol - outMass);

	float4 gradient   = outMass * inSpace - inMass * outSpace;
	float4 volumeFlux = clamp(gradient, -kSurfaceFlux * inSpace, kSurfaceFlux * outSpace);

	float4 newQuantity = q;
	newQuantity.w += dot(float4(1, 1, 1, 1), volumeFlux);
	return newQuantity;
}
