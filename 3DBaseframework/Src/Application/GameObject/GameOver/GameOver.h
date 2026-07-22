#pragma once

class GameOver : public KdGameObject
{
public:

	GameOver() {}
	~GameOver() {}

	void Init()override;
	void Draw();

private:

	std::shared_ptr<KdSquarePolygon> m_polygon;

};