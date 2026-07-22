#pragma once

// ===================================================
// 水（流体表現）
//  コップ内に溜まる水位と、揺れる水面を2Dスプライトで描画する
//  ※水流・飛沫は今後この中に足していく想定
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
	void Reset() { m_fillRate = 0.0f; }

private:
	// 水位（0.0～1.0）
	float m_fillRate = 0.0f;

	// 注水中フラグ
	bool m_isPouring = false;
};
