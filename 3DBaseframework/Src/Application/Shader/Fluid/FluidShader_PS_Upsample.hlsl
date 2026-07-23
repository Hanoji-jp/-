// ===================================================
// 流体 圧力：upsample（粗→細の圧力補正）
//  みずあそび.html の upsample-fragment を移植。
//  粗いレベルの圧力(w)だけを取り出して出力する。
//  呼び出し側で純加算ブレンドにより細いレベルへ加算される（xyzは0）。
// ===================================================

Texture2D<float4> g_intensity : register(t0);	// 粗いレベル

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy) / 2;
	float4 it = g_intensity.Load(int3(c, 0));
	return float4(0.0, 0.0, 0.0, it.w);
}
