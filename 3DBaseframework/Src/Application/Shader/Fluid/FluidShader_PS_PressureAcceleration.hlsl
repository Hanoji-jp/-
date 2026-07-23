// ===================================================
// 流体 圧力：pressure-acceleration（圧力勾配で加速）
//  みずあそび.html の pressure-acceleration-fragment を移植。
//  connection を [-1,1] にクランプし、質量に比例して運動量へ加える。
// ===================================================

Texture2D<float4> g_quantity   : register(t0);
Texture2D<float4> g_connection : register(t1);

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q    = g_quantity.Load(int3(c, 0));
	float4 conn = g_connection.Load(int3(c, 0));

	float4 acceleration = clamp(conn, -1.0, 1.0);

	float4 newQuantity = q;
	// xy=(-X,-Y方向), zw=(+X,+Y方向) の差し引きが正味の力
	newQuantity.xy += q.z * (acceleration.xy - acceleration.zw);
	return newQuantity;
}
