#pragma once

class GameOver : public KdGameObject
{
public:

	GameOver() {}
	~GameOver() {}

	void Init()override;
	void DrawLit()override;

private:

	std::shared_ptr<KdSquarePolygon> m_polygon;
	std::shared_ptr<KdSquarePolygon> m_darkOverlay;
};