// ===================================================
// 流体 質量正規化（保存の強制）
//  数値拡散などで GPU 側の総質量がドリフトしても、CPUが把握している
//  「注いだ総量」に一致するよう、質量(z)と運動量(xy)を一律スケールする。
//  velocity=xy/z を保つため xy も同じ率で掛ける。体積(w)は変えない。
// ===================================================

Texture2D<float4> g_quantity : register(t0);

cbuffer cbMassScale : register(b0)
{
	float  g_scale;
	float3 _pad;
};

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));
	q.xyz *= g_scale;	// 質量と運動量を同率でスケール（速度は不変）
	return q;
}
