#include "GameOver.h"

void GameOver::Init()
{
	// 文字
	m_polygon = std::make_shared<KdSquarePolygon>();
	m_polygon->SetMaterial("Asset/Textures/sipai.png");

	// 黒背景
	m_darkOverlay = std::make_shared<KdSquarePolygon>();
	m_darkOverlay->SetMaterial("Asset/Textures/kuro.png");
	m_darkOverlay->SetScale(100.0f);
	std::vector<Math::Color> darkColors(4, Math::Color(0.0f, 0.0f, 0.0f, 0.8f));
	m_darkOverlay->SetVertexColor(darkColors);
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

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_polygon, m_mWorld);
}
