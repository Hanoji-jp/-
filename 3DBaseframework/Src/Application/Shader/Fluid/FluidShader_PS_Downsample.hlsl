// ===================================================
// 流体 圧力：downsample（細→粗のリストリクション）
//  みずあそび.html の downsample-fragment を移植。
//  細いレベル(入力)の2x2ブロック＋周辺から、粗いレベルの
//  結合重み・source(残差を含む)を作る。圧力(w)は0で初期化。
// ===================================================

Texture2D<float4> g_intensity : register(t0);	// 細いレベル

float4 main(float4 svPos : SV_Position) : SV_Target
{
	uint W, H;
	g_intensity.GetDimensions(W, H);
	int2 maxCoord = int2(W - 1, H - 1);

	int2 f00 = int2(svPos.xy) * 2;
	int2 f10 = f00 + int2(1, 0);
	int2 f01 = f00 + int2(0, 1);
	int2 f11 = f00 + int2(1, 1);
	int2 f_0 = max(f00 + int2(-1, 0), int2(0, 0));
	int2 f_1 = max(f00 + int2(-1, 1), int2(0, 0));
	int2 f20 = min(f00 + int2(2, 0), maxCoord);
	int2 f21 = min(f00 + int2(2, 1), maxCoord);
	int2 f0_ = max(f00 + int2(0, -1), int2(0, 0));
	int2 f1_ = max(f00 + int2(1, -1), int2(0, 0));
	int2 f02 = min(f00 + int2(0, 2), maxCoord);
	int2 f12 = min(f00 + int2(1, 2), maxCoord);

	float4 i00 = g_intensity.Load(int3(f00, 0));
	float4 i10 = g_intensity.Load(int3(f10, 0));
	float4 i01 = g_intensity.Load(int3(f01, 0));
	float4 i11 = g_intensity.Load(int3(f11, 0));
	float4 i_0 = g_intensity.Load(int3(f_0, 0));
	float4 i_1 = g_intensity.Load(int3(f_1, 0));
	float4 i20 = g_intensity.Load(int3(f20, 0));
	float4 i21 = g_intensity.Load(int3(f21, 0));
	float4 i0_ = g_intensity.Load(int3(f0_, 0));
	float4 i1_ = g_intensity.Load(int3(f1_, 0));
	float4 i02 = g_intensity.Load(int3(f02, 0));
	float4 i12 = g_intensity.Load(int3(f12, 0));

	float weightX = 0.5 * (i00.x + i01.x);
	float weightY = 0.5 * (i00.y + i10.y);
	float source  = i00.z + i10.z + i01.z + i11.z;

	float source_0 = i00.x * (i_0.w - i00.w);
	float source_1 = i01.x * (i_1.w - i01.w);
	float source20 = i20.x * (i20.w - i10.w);
	float source21 = i21.x * (i21.w - i11.w);
	float source0_ = i00.y * (i0_.w - i00.w);
	float source1_ = i10.y * (i1_.w - i10.w);
	float source02 = i02.y * (i02.w - i01.w);
	float source12 = i12.y * (i12.w - i11.w);

	float sourceX = source_0 + source_1 + source20 + source21;
	float sourceY = source0_ + source1_ + source02 + source12;

	return float4(weightX, weightY, source + sourceX + sourceY, 0.0);
}
