// ===================================================
// 流体 圧力：pressure-displacement（非圧縮化のための質量再配置）
//  みずあそび.html の pressure-displacement-32-fragment を移植。
//  connection(圧力勾配)に基づく flow で、近傍と物理量を出入りさせる。
// ===================================================

Texture2D<float4> g_quantity   : register(t0);
Texture2D<float4> g_connection : register(t1);

static const float kMinVolume = 0.001;

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q   = g_quantity.Load(int3(c, 0));
	float4 qpx = g_quantity.Load(int3(c - int2(1, 0), 0));
	float4 qpy = g_quantity.Load(int3(c - int2(0, 1), 0));
	float4 qnx = g_quantity.Load(int3(c + int2(1, 0), 0));
	float4 qny = g_quantity.Load(int3(c + int2(0, 1), 0));
	float4 conn = g_connection.Load(int3(c, 0));

	float  inVolume  = q.w;
	float4 outVolume = float4(qpx.w, qpy.w, qnx.w, qny.w);
	float4 sumVolume = inVolume + outVolume;
	// connが正の向き(隣へ流出)は隣の体積を、負なら自分の体積を基準にする
	float4 upVolume  = lerp(float4(inVolume, inVolume, inVolume, inVolume), outVolume, step(0.0, conn));
	float4 flow = clamp(sumVolume / max(upVolume, kMinVolume) * conn, -0.25, 0.25);

	// inFlux = mat4(qpx,qpy,qnx,qny) * max(flow,0)  … 各近傍を列ベクトルとした行列×ベクトル
	float4 inFlow = max(flow, 0.0);
	float4 inFlux = qpx * inFlow.x + qpy * inFlow.y + qnx * inFlow.z + qny * inFlow.w;
	float4 outFlux = q * dot(float4(1, 1, 1, 1), max(-flow, 0.0));

	return q + inFlux - outFlux;
}
