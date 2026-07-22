#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/GameOver/GameOver.h"

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}

void GameScene::Init()
{
	// ゲームオーバー
	std::shared_ptr<GameOver>gameover;
	gameover = std::make_shared<GameOver>();
	gameover->Init();
	m_objList.push_back(gameover);
}