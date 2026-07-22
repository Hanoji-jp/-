// ===================================================
// 流体シミュレーション用 フルスクリーン頂点シェーダ
//  クリップ空間の4頂点(トライアングルストリップ)をそのまま出力するだけ。
//  各パスのピクセルシェーダは SV_Position からグリッド座標を得る。
// ===================================================

struct VSOutput
{
	float4 Pos : SV_Position;
};

VSOutput main(float3 pos : POSITION)
{
	VSOutput output;
	output.Pos = float4(pos, 1.0f);
	return output;
}
