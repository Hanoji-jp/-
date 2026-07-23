#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Effect/Clear/RyouEffect/RyouEffect.h"

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}

	// カメラの処理
	Math::Vector3 _camPos = { 0.0f, 0.0f, 0.0f };

	// 行列
	Math::Matrix transMat = Math::Matrix::CreateTranslation(_camPos);
	m_camera->SetCameraMatrix(transMat);
}

void GameScene::Init()
{
	// カメラを用意する
	m_camera = std::make_unique<KdCamera>();

	//====================
	// オブジェクト
	//====================
	//----- エフェクト -----
	// 良
	std::shared_ptr<RyouEffect> _spRyou;
	_spRyou = std::make_shared<RyouEffect>();
	m_objList.push_back(_spRyou);
	_spRyou->Init();
}
