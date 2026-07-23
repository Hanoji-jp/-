// ===================================================
// 炭酸：泡の浮上＋水平拡散
//  泡は液体の中を上へ抜け、空気に達したら留まる（＝液面に頭を作る）。
//  さらに左右へ拡散して頭を平らにする。
//  row0=上、row増加=下。上=row-1、下=row+1。
// ===================================================

Texture2D<float4> g_foam     : register(t0);
Texture2D<float4> g_quantity : register(t1);

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

float FoamAt(int2 c) { return g_foam.Load(int3(c, 0)).r; }
float MassAt(int2 c) { return g_quantity.Load(int3(c, 0)).z; }

float4 main(float4 svPos : SV_Position) : SV_Target
{
	uint W, H;
	g_foam.GetDimensions(W, H);
	int2 c = int2(svPos.xy);

	float F = FoamAt(c);
	float massHere = MassAt(c);
	float massUp   = (c.y > 0)            ? MassAt(c - int2(0, 1)) : 0.0;	// 上（空気なら0扱い）

	// 上が液体なら泡は上へ抜ける。上が空気なら留まる（液面）
	float riseOut = F * g_riseRate * step(g_massThresh, massUp);

	// 下の泡が、ここが液体なら上がってくる
	float Fdown = (c.y < (int)H - 1) ? FoamAt(c + int2(0, 1)) : 0.0;
	float riseIn = Fdown * g_riseRate * step(g_massThresh, massHere);

	float newF = F - riseOut + riseIn;

	// 水平拡散（頭を平らに）
	float Fl = (c.x > 0)          ? FoamAt(c - int2(1, 0)) : F;
	float Fr = (c.x < (int)W - 1) ? FoamAt(c + int2(1, 0)) : F;
	newF += g_spreadRate * (Fl + Fr - 2.0 * F);

	return float4(max(newF, 0.0), 0.0, 0.0, 0.0);
}
