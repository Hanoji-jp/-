// ===================================================
// コップ形状マスクの強制
//  solid=0（コップの外＝壁）のセルは物理量を0にする。
//  これで水はコップ多角形の内側にだけ存在できる。
// ===================================================

Texture2D<float4> g_quantity : register(t0);
Texture2D<float4> g_solid    : register(t1);	// r: 1=内側(水が入れる) / 0=壁

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));
	float  s = g_solid.Load(int3(c, 0)).r;
	return q * s;
}
