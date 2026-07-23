#include "GameUI.h"
#include "../Water/Water.h"

void GameUI::Init()
{
	// TODO: 判定結果の初期化
}

void GameUI::Update()
{
	// 注水終了後、Water が確定した判定結果を受け取る
	if (auto spWater = m_wpWater.lock())
	{
		m_result = spWater->GetResult();
	}
}

void GameUI::DrawSprite()
{
	// TODO: m_result に応じて「良！」などの演出を2D描画（フォント素材は今後）
}
