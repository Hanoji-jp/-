#include "GameOver.h"

void GameOver::Init()
{
	m_polygon = std::make_shared<KdSquarePolygon>();

	m_polygon->SetMaterial("Asset/Textures/kusa.png");
}

void GameOver::Draw()
{
	if (!m_polygon) return;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_polygon, m_mWorld);
}
