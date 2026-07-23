#include "Desk.h"

void Desk::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();

		m_spModel->Load("Asset/Data/Models/desk/desk.gltf");

		Math::Matrix transMat = Math::Matrix::CreateTranslation(0, -13, -2);
		Math::Matrix scaleMat = Math::Matrix::CreateScale(10.0f);
		m_mWorld = scaleMat * transMat;
	}
}

void Desk::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
