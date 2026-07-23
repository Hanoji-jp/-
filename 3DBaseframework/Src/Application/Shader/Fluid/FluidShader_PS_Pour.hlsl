// ===================================================
// 流体 注水（pour）
//  上部中央の吸い込み口(矩形)に、水（質量・体積）と下向き運動量を加える。
//  SPACE押下中のみ呼ばれる。CPU側で注いだ量を積算し水位判定に使う。
// ===================================================

Texture2D<float4> g_quantity : register(t0);

cbuffer cbPour : register(b0)
{
	float4 g_inletRect;	// 吸い込み口 (colMin, rowMin, colMax, rowMax)
	float  g_massRate;	// 1ステップで加える質量（セルあたり）
	float  g_velocity;	// 加える下向き速度（row増加方向）
	float2 _pad;
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));

	if (c.x >= (int)g_inletRect.x && c.x < (int)g_inletRect.z &&
		c.y >= (int)g_inletRect.y && c.y < (int)g_inletRect.w)
	{
		q.z += g_massRate;					// 質量
		q.w += g_massRate;					// 体積
		q.y += g_massRate * g_velocity;		// 下向き運動量（mass×velocity）
	}
	return q;
}
