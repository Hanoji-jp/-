#include "GameScene.h"
#include"../SceneManager.h"

#include "../../GameObject/Camera/CameraBase.h"
#include "../../GameObject/Cup/Cup.h"
#include "../../GameObject/Cup/CupConst.h"
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

	if (auto spWater = m_wpWater.lock())
	{
		// スペースキーを押している間だけ水を注ぐ
		const bool isPouring = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
		spWater->SetPouring(isPouring);

		// Rキーで水位をリセット
		if (GetAsyncKeyState('R') & 0x8000)
		{
			spWater->Reset();
		}
	}
}

void GameScene::Init()
{
	// コップを正面から見るカメラ
	std::shared_ptr<CameraBase> spCamera = std::make_shared<CameraBase>();
	spCamera->Init();
	// DirectXは左手座標系でカメラ正面が+Z。原点を見るためカメラは-Z側に置く
	spCamera->SetPos({ CupConst::kCenterX, 0.0f, -CupConst::kCameraDistanceZ });
	AddObject(spCamera);

	// 水（簡易2Dグリッド流体）
	std::shared_ptr<Water> spWater = std::make_shared<Water>();
	spWater->Init();
	AddObject(spWater);
	m_wpWater = spWater;

	// コップ（枠・目標ライン）：水より後に描いて線を上に出す
	std::shared_ptr<Cup> spCup = std::make_shared<Cup>();
	spCup->Init();
	AddObject(spCup);

	// 判定演出UI
	std::shared_ptr<GameUI> spGameUI = std::make_shared<GameUI>();
	spGameUI->Init();
	spGameUI->SetTargetWater(spWater);
	AddObject(spGameUI);
}
