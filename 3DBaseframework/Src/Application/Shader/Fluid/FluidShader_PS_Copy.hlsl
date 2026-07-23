// ===================================================
// 流体シミュレーション 恒等コピーパス
//  quantity テクスチャ(xy=運動量, z=質量, w=体積)を、そのまま出力する。
//  Phase1では float レンダーターゲットの読み書き＋ping-pongの疎通確認に使う。
//  ※テクセルを整数座標でそのまま取得したいので Load を使用(サンプラ不要)。
// ===================================================

Texture2D<float4> g_quantity : register(t0);

float4 main(float4 svPos : SV_Position) : SV_Target
{
	int2 coord = int2(svPos.xy);
	return g_quantity.Load(int3(coord, 0));
}
