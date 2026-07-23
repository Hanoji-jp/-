#pragma once

class GameOver : public KdGameObject
{
public:

	GameOver() {}
	~GameOver() {}

	void Init()override;
	void Update()override;
	void DrawLit()override;

private:

	enum class Phase
	{
		TextFalling, // 「終了」の文字が降ってくる最中
		ShowResult   // 着地後、リザルトを明滅表示
	};
	Phase m_phase = Phase::TextFalling;

	std::shared_ptr<KdSquarePolygon> m_polygon;
	std::shared_ptr<KdSquarePolygon> m_darkOverlay;
	std::shared_ptr<KdSquarePolygon> m_result;

	Math::Vector3 m_textOffset = { 0.0f, 0.0f, 0.0f };
	Math::Vector3 m_resultOffset = { 0.0f,0.0f,0.0f };

	int m_frame = 0;

	// 落下演出パラメータ
	int   m_fallDuration = 120;   // 何フレームかけて降りるか（速さ)
	float m_textStartY = 2.0f; // 降り始めの高さ
	float m_textEndY = 0.4f; // 最終的な位置（元の値）
};