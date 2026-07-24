#include "GameScene.h"
#include"../SceneManager.h"

#include "../../GameObject/Camera/CameraBase.h"
#include "../../GameObject/Cup/Cup.h"
#include "../../GameObject/Cup/CupConst.h"
#include "../../GameObject/Water/Water.h"
#include "../../GameObject/UI/GameUI.h"
#include"../../GameObject/GameOver/GameOver.h"
#include"../../GameObject/GameStart/GameStart.h"
#include"../../GameObject/Tension/Tension.h"
#include"../../GameObject/Effect/ClearEffectManager/ClearEffectManager.h"
#include "../../GameObject/Desk/Desk.h"
#include "../../GameObject/DrinkBar/DrinkBar.h"
#include "../../GameObject/Tuning/Tuning.h"
#include "../../GameObject/StandLight/StandLight.h"
#include "../../GameObject/Wall/Wall.h"
#include "../../GameObject/Window/Window.h"

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
		const bool spaceDown = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

		// タイトルからSPACEで入場した直後、押しっぱなしのSPACEでいきなり注水＝即判定に
		// なってしまうのを防ぐ。一度SPACEが離されるまで注水を受け付けない。
		if (!spaceDown) { m_pourArmed = true; }

		// 開始演出（水をドンピシャで入れろ→3・2・1→始めっ）が終わるまでは注水させない。
		bool introDone = true;
		if (auto spStart = m_wpGameStart.lock()) { introDone = spStart->IsFinished(); }

		const bool isPouring = spaceDown && m_pourArmed && introDone;
		spWater->SetPouring(isPouring);

		// Rキーで水位をリセット（結果演出も消す）。開始演出（3・2・1・start）中は操作無効。
		if (introDone && (GetAsyncKeyState('R') & 0x8000))
		{
			spWater->Reset();
			if (auto spGameOver = m_wpGameOver.lock()) { spGameOver->Deactivate(); }
			if (auto spRyou = m_wpClrEftMng.lock()) { spRyou->Deactivate(); }
		}

		// 注ぎ終わり（一発勝負確定）で結果に応じて演出を出す
		if (spWater->IsPourFinished())
		{
			const UIConst::RyoResult result = spWater->GetResult();
			if (result == UIConst::RyoResult::Ryo)
			{
				// ぴったり（良）→ 良演出
				if (auto spRyou = m_wpClrEftMng.lock()) { spRyou->Activate(); }
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
	// 保存済みの調整値（コップ／水の出る位置／ドリンクバー）をCSVから読み込む
	Tuning::LoadCsv();

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

	// 緊張演出（心臓音＋コップへズーム＋上下の黒帯を縮める）。水位が目標へ近づくほど強くなる。
	std::shared_ptr<Tension> spTension = std::make_shared<Tension>();
	spTension->Init();
	spTension->SetCamera(spCamera);
	spTension->SetWater(spWater);
	AddObject(spTension);

	// コップ（枠・目標ライン）：水より後に描いて線を上に出すaddObject(spWater);
	std::shared_ptr<Cup> spCup = std::make_shared<Cup>();
	spCup->Init();
	AddObject(spCup);

	// 判定演出UI
	std::shared_ptr<GameUI> spGameUI = std::make_shared<GameUI>();
	spGameUI->Init();
	spGameUI->SetTargetWater(spWater);
	AddObject(spGameUI);

	// 机（コップの土台）：GameOverの全画面暗幕より先に描くため、GameOverより前に追加する
	std::shared_ptr<Desk> spDesk = std::make_shared<Desk>();
	spDesk->Init();
	AddObject(spDesk);

	// ドリンクバー（ディスペンサー）：ノズルの下にコップを置いて水を注ぐ
	std::shared_ptr<DrinkBar> spDrinkBar = std::make_shared<DrinkBar>();
	spDrinkBar->Init();
	AddObject(spDrinkBar);

	//====================
	// オブジェクト
	//====================
	//----- エフェクト -----
	// クリアエフェクトマネージャー
	std::shared_ptr<ClearEffectManager> _spClrEftMng;
	_spClrEftMng = std::make_shared<ClearEffectManager>();
	m_objList.push_back(_spClrEftMng);
	_spClrEftMng->Init();
	_spClrEftMng->SetOwner(this);
	m_wpClrEftMng = _spClrEftMng;

	// ゲームオーバー（失敗時のみ表示。初期は無効）
	//  全画面の暗幕なので、シーンの3D物より後（最後）に描く必要がある。
	std::shared_ptr<GameOver> gameover = std::make_shared<GameOver>();
	gameover->Init();
	m_objList.push_back(gameover);
	m_wpGameOver = gameover;

	// 机（インテリア）
	std::shared_ptr<StandLight> spLight = std::make_shared<StandLight>();
	spLight->Init();
	AddObject(spLight);

	// 壁（インテリア）
	std::shared_ptr<Wall> spWall = std::make_shared<Wall>();
	spWall->Init();
	AddObject(spWall);

	// 窓（インテリア）
	std::shared_ptr<Window> spWindow = std::make_shared<Window>();
	spWindow->Init();
	AddObject(spWindow);

	// 開始演出（3→2→1→水をドンピシャで入れろ）。最後に追加＝スプライトが最前面に出る。
	//  これが終わるまで注水は受け付けない（Eventでゲート）。
	std::shared_ptr<GameStart> spGameStart = std::make_shared<GameStart>();
	spGameStart->Init();
	AddObject(spGameStart);
	m_wpGameStart = spGameStart;

	// 開始演出が終わったら心臓音を鳴らすため、Tension に開始演出を渡す
	spTension->SetGameStart(spGameStart);
}