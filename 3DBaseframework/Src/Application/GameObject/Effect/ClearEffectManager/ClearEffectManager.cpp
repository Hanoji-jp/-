#include "ClearEffectManager.h"
#include "../../../Scene/GameScene/GameScene.h"

// 各エフェクトインクルード
#include "../BlackoutEffect/BlackoutEffect.h"
#include "../RainbowBack/RainbowBack.h"
#include "../Clear/RyouEffect/RyouEffect.h"

void ClearEffectManager::Init()
{
	// アクティブフラグ
	m_activeFlg = false;

	// クリアしてからの時間（このオブジェクトがアクティブ化してからの時間）
	m_ClearTime = 0;

	EffectMaterial();
}

void ClearEffectManager::Update()
{
	// 非アクティブ状態なら処理しない
	if (!m_activeFlg) { return; }

	// エフェクト生成
	if (m_ClearTime == 0)
	{	
		// 暗転オブジェクト生成 & 初期化してリストへ追加
		m_spBlackout = std::make_shared<BlackoutEffect>();	// 生成
		m_spBlackout->Init();								// 初期化
		m_owner->AddObject(m_spBlackout);					// シーンのオブジェクトリストへ追加
		m_spBlackout->Activate();							// アクティブ化
	}
	else if (m_ClearTime == 60)
	{
		if (m_spBlackout)
		{
			m_spBlackout->Deactivate();
		}

		// 虹背景オブジェクト生成 & 初期化してリストへ追加
		m_spRainbowBack = std::make_shared<RainbowBack>();	// 生成
		m_spRainbowBack->Init();							// 初期化
		m_owner->AddObject(m_spRainbowBack);				// シーンのオブジェクトリストへ追加
		m_spRainbowBack->Activate();						// アクティブ化

		// 良オブジェクト生成 & 初期化してリストへ追加
		m_spRyou = std::make_shared<RyouEffect>();	// 生成
		m_spRyou->Init();							// 初期化
		m_owner->AddObject(m_spRyou);				// シーンのオブジェクトリストへ追加
		m_spRyou->Activate();						// アクティブ化
	}
	else if (m_ClearTime == 1260)
	{
		Deactivate();
	}
	
	// クリアエフェクトカウント
	m_ClearTime++;
}

void ClearEffectManager::EffectMaterial()
{
	

	
}

void ClearEffectManager::Deactivate()
{
	// 全エフェクト非アクティブ化
	if (m_spBlackout)
	{
		m_spBlackout->Deactivate();
	}

	if (m_spRyou)
	{
		m_spRyou->Deactivate();
	}

	if (m_spRainbowBack)
	{
		m_spRainbowBack->Deactivate();
	}

	m_activeFlg = false;
}
