// ===================================================
// 流体 equalization（質量・体積の是正：非圧縮と拡散の補正）
//  ・体積 w は常に 1 へ緩和（非圧縮の維持）
//  ・余剰(w>1)は内容に比例して削る
//  ・不足(w<1)の「質量」補充は “すでに水のセル” だけに限定する
//    （空気セルに幻の水を足さない＝浮遊ノイズを防ぐ）。
//  これで移流の数値拡散で薄まった水を濃く保ちつつ、水量が減らず、空気も汚れない。
// ===================================================

Texture2D<float4> g_quantity : register(t0);

cbuffer cbEqualization : register(b0)
{
	float  g_surplusRate;	// 余剰を削る率
	float  g_deficitRate;	// 不足を補う率
	float2 _pad;
	float4 g_deficitRatio;	// 補充で加える比率 (0, 0, 補充密度, 1)
};

static const float kWaterGate = 0.25;	// これ未満の質量は「水ではない」＝質量補充しない

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 c = int2(svPos.xy);
	float4 q = g_quantity.Load(int3(c, 0));

	// 余剰(w>1)は内容に比例して削る（質量・体積とも）
	float4 surplusRatio = q / max(q.w, 1.0);
	q -= g_surplusRate * max(q.w - 1.0, 0.0) * surplusRatio;

	// 不足(w<1)を補う：体積は常に1へ、質量は水セルのみ濃く戻す
	float deficit = g_deficitRate * max(1.0 - q.w, 0.0);
	float waterGate = saturate(q.z / kWaterGate);	// 空気(低質量)は0＝質量を足さない
	q.z += deficit * g_deficitRatio.z * waterGate;
	q.w += deficit * g_deficitRatio.w;

	q.zw = max(q.zw, float2(0.0, 0.0));
	return q;
}
