#include "GameScene.h"
#include"../SceneManager.h"

#include "../../GameObject/Camera/CameraBase.h"
#include "../../GameObject/Cup/Cup.h"
#include "../../GameObject/Cup/CupConst.h"
#include "../../GameObject/Water/Water.h"
#include "../../GameObject/UI/GameUI.h"
#include"../../GameObject/GameOver/GameOver.h"
#include"../../GameObject/Effect/Clear/RyouEffect/RyouEffect.h"
#include "../../GameObject/Desk/Desk.h"
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

		// Rキーで水位をリセット（結果演出も消す）
		if (GetAsyncKeyState('R') & 0x8000)
		{
			spWater->Reset();
			if (auto spGameOver = m_wpGameOver.lock()) { spGameOver->Deactivate(); }
			if (auto spRyou = m_wpRyou.lock())         { spRyou->Deactivate(); }
		}

		// 注ぎ終わり（一発勝負確定）で結果に応じて演出を出す
		if (spWater->IsPourFinished())
		{
			const UIConst::RyoResult result = spWater->GetResult();
			if (result == UIConst::RyoResult::Ryo)
			{
				// ぴったり（良）→ 良演出
				if (auto spRyou = m_wpRyou.lock()) { spRyou->Activate(); }
			}
			else if (result == UIConst::RyoResult::Under || result == UIConst::RyoResult::Over)
			{
				// ラインに合わなかった → ゲームオーバー
				if (auto spGameOver = m_wpGameOver.lock()) { spGameOver->Activate(); }
			}
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

	// コップ（枠・目標ライン）：水より後に描いて線を上に出すaddObject(spWater);
	std::shared_ptr<Cup> spCup = std::make_shared<Cup>();
	spCup->Init();
	AddObject(spCup);

	// 判定演出UI
	std::shared_ptr<GameUI> spGameUI = std::make_shared<GameUI>();
	spGameUI->Init();
	spGameUI->SetTargetWater(spWater);
	AddObject(spGameUI);
	// ゲームオーバー（失敗時のみ表示。初期は無効）
	std::shared_ptr<GameOver> gameover = std::make_shared<GameOver>();
	gameover->Init();
	m_objList.push_back(gameover);
	m_wpGameOver = gameover;
	//====================
		// オブジェクト
		//====================
		//----- エフェクト -----
		// 良
	std::shared_ptr<RyouEffect> _spRyou;
	_spRyou = std::make_shared<RyouEffect>();
	m_objList.push_back(_spRyou);
	_spRyou->Init();
	m_wpRyou = _spRyou;

	// 机（コップの土台）
	std::shared_ptr<Desk> spDesk = std::make_shared<Desk>();
	spDesk->Init();
	AddObject(spDesk);
}


