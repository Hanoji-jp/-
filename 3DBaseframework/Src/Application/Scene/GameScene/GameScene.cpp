#include "GameScene.h"
#include"../SceneManager.h"

#include "../../GameObject/Cup/Cup.h"
#include "../../GameObject/Water/Water.h"
#include "../../GameObject/UI/GameUI.h"

void GameScene::Event()
{
	// タイトルへ戻る
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}

	// TODO: 注水などの入力処理
}

void GameScene::Init()
{
	// コップ
	std::shared_ptr<Cup> spCup = std::make_shared<Cup>();
	spCup->Init();
	AddObject(spCup);

	// 水（流体表現）
	std::shared_ptr<Water> spWater = std::make_shared<Water>();
	spWater->Init();
	AddObject(spWater);
	m_wpWater = spWater;

	// 判定演出UI
	std::shared_ptr<GameUI> spGameUI = std::make_shared<GameUI>();
	spGameUI->Init();
	spGameUI->SetTargetWater(spWater);
	AddObject(spGameUI);
}
