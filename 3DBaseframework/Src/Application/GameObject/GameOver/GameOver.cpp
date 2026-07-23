#include "GameOver.h"

void GameOver::Init()
{
	// 文字（終了）
	m_polygon = std::make_shared<KdSquarePolygon>();
	m_polygon->SetMaterial("Asset/Textures/gameover.png");

	// 大きさと位置（終了）
	m_polygon->SetScale(1.2f);
	m_textOffset = { 0.0f, 0.4f, 0.0f };

	// たいとる,やりなおし
	m_result = std::make_shared<KdSquarePolygon>();
	m_result->SetMaterial("Asset/Textures/resultUI.png");

	// 大きさと位置（たいとる,やりなおし）
	m_result->SetScale(1.0f);
	m_resultOffset = { -0.1f,-0.3f,0.0f };

	// 黒背景
	m_darkOverlay = std::make_shared<KdSquarePolygon>();
	m_darkOverlay->SetMaterial("Asset/Textures/kuro.png");

	// 大きさと透明度（黒背景）
	m_darkOverlay->SetScale(100.0f);
	std::vector<Math::Color> darkColors(4, Math::Color(0.0f, 0.0f, 0.0f, 0.6f));
	m_darkOverlay->SetVertexColor(darkColors);
}

void GameOver::Update()
{
	// リザルトの明滅
	if (!m_result) return;

	m_frame++;

	float t = (std::sinf(m_frame * 0.05f) + 1.0f) * 0.5f;

	// 明滅範囲（完全に消したいなら 0.0f に変更)
	float alpha = 0.2f + 0.7f * t;

	std::vector<Math::Color> resultColors(4, Math::Color(0.0f, 0.0f, 0.0f, alpha));
	m_result->SetVertexColor(resultColors);
}

void GameOver::DrawLit()
{
	if (!m_polygon) return;

	if (m_darkOverlay)
	{
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_darkOverlay, m_mWorld);

		KdShaderManager::Instance().UndoBlendState();
	}
	Math::Matrix textWorld = m_mWorld * Math::Matrix::CreateTranslation(m_textOffset);
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_polygon, textWorld);
	
	Math::Matrix resultWorld = m_mWorld * Math::Matrix::CreateTranslation(m_resultOffset);
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_result, resultWorld);
}
