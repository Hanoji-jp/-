#pragma once

#include"../BaseScene/BaseScene.h"

// 前方宣言
class Water;

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

private:

	void Event() override;
	void Init()  override;
	void DrawSprite() override {}

	// 注水を操作する対象の水
	std::weak_ptr<Water> m_wpWater;
};
