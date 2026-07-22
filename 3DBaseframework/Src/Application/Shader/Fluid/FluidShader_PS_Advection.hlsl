// ===================================================
// 流体シミュレーション 移流パス（ピクセルシェーダ）
//  みずあそび.html の advection-32-fragment を移植。
//  撒き込み中心(NewPos)からの距離で二次線形の重みを付け、
//  純加算ブレンド(ONE,ONE)で近傍セルへ物理量を配分する（質量保存）。
// ===================================================

struct GSOutput
{
	float4 Pos    : SV_Position;
	float2 NewPos : TEXCOORD0;
	float4 NewQ   : TEXCOORD1;
};

float4 main(GSOutput input) : SV_Target
{
	float2 distance = clamp(input.Pos.xy - input.NewPos, -1.0, 1.0);
	float2 weight = 1.0 - abs(distance);
	return weight.x * weight.y * input.NewQ;
}
