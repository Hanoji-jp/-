#pragma once

#include"../BaseScene/BaseScene.h"

// 前方宣言
class Water;
class GameOver;
class RyouEffect;

class GameScene : public BaseScene
{
public:

	GameScene() { Init(); }
	~GameScene() {}

private:

	void Event() override;
	void Init()  override;
	void DrawSprite() override { BaseScene::DrawSprite(); }

	// 注水を操作する対象の水
	std::weak_ptr<Water> m_wpWater;

	// 失敗時に表示するゲームオーバー
	std::weak_ptr<GameOver> m_wpGameOver;

	// 成功（良）時に表示する演出
	std::weak_ptr<RyouEffect> m_wpRyou;
};
