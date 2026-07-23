// ===================================================
// 流体 圧力：compression（intensityピラミッド最上段の構築）
//  みずあそび.html の compression-fragment を移植。
//  quantity から、-X/-Y方向の結合重み・体積超過(source)・圧力(0)を作る。
//   intensity = (weightX, weightY, source=volume-1, pressure=0)
// ===================================================

Texture2D<float4> g_quantity : register(t0);

static const float kMinMass = 0.001;

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q   = g_quantity.Load(int3(c, 0));
	float4 qpx = g_quantity.Load(int3(c - int2(1, 0), 0)) + q;
	float4 qpy = g_quantity.Load(int3(c - int2(0, 1), 0)) + q;

	float weightX = (c.x > 0) ? qpx.w / max(qpx.z, kMinMass) : 0.0;
	float weightY = (c.y > 0) ? qpy.w / max(qpy.z, kMinMass) : 0.0;

	return float4(weightX, weightY, q.w - 1.0, 0.0);
}
