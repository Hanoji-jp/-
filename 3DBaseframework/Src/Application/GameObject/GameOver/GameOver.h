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

	std::shared_ptr<KdSquarePolygon> m_polygon;
	std::shared_ptr<KdSquarePolygon> m_darkOverlay;
	std::shared_ptr<KdSquarePolygon> m_result;

	Math::Vector3 m_textOffset = { 0.0f, 0.0f, 0.0f };
	Math::Vector3 m_resultOffset = { 0.0f,0.0f,0.0f };

	int m_frame = 0;
};