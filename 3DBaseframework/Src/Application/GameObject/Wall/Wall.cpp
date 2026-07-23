#include "Wall.h"

void Wall::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();

		m_spModel->Load("Asset/Data/Models/wall/wall.gltf");

		Math::Matrix transMat = Math::Matrix::CreateTranslation(0, -500, 90);
		Math::Matrix scaleMat = Math::Matrix::CreateScale(500.0f, 500.0f, 10);
		m_mWorld = scaleMat * transMat;
	}
}

void Wall::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
