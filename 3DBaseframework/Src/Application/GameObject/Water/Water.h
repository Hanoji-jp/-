#pragma once

// ===================================================
// 水面板ポリゴン
//  各列の高さ配列から、三角形ストリップの頂点を組み立てる
// ===================================================
class WaterSurfacePolygon : public KdPolygon
{
public:
	WaterSurfacePolygon() { Set2DObject(false); }	// 3D空間の板ポリとして扱う

	// 高さ配列から水面の頂点を更新する
	//  heights      … 各列の水面高さ（底からの高さ）
	//  leftX        … 一番左の列のワールドX
	//  columnStride … 列と列の間隔（ワールド）
	//  bottomY      … 水の底のワールドY
	void UpdateSurface(const std::vector<float>& heights, float leftX, float columnStride, float bottomY);
};

// ===================================================
// 水（簡易2Dグリッド流体：1次元高さ場／浅水近似）
//  コップ内に溜まる水位と、揺れる水面を板ポリゴンで表現する
// ===================================================
class Water : public KdGameObject
{
public:
	Water()						{}
	~Water()		override	{}

	void Init()		override;
	void Update()	override;
	void DrawLit()	override;

	// 注いでいる最中かどうかを設定
	void SetPouring(bool pouring) { m_isPouring = pouring; }

	// 現在の水位（0.0=空 ～ 1.0=内側領域の上端）
	float GetFillRate() const { return m_fillRate; }

	// 水位をリセット
	void Reset();

private:
	// 高さ場シミュレーションを1ステップ進める
	void StepSimulation(float deltaTime);

	// 各列の水面高さ（底からの高さ）
	std::vector<float> m_heights;

	// 各列の速度
	std::vector<float> m_velocities;

	// 平均水位（0.0～1.0）
	float m_fillRate = 0.0f;

	// 注水中フラグ
	bool m_isPouring = false;

	// 描画用の水面板ポリゴン
	WaterSurfacePolygon m_surface;
};
