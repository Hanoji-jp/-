#include "ClearEffectManager.h"
#include "../../../Scene/GameScene/GameScene.h"
#include "../Clear/RyouEffect/RyouEffect.h"

void ClearEffectManager::Init()
{
	m_activeFlg = false;

	m_ClearTime = 0;
}

void ClearEffectManager::Update()
{
	// 非アクティブ状態なら処理しない
	if (!m_activeFlg) { return; }

	// 暗転エフェクトアクティブ化
	if (m_ClearTime == 0)
	{

	}

	// 暗転エフェクト終了後良エフェクト実行
	//if ()

}
