#include "StandLight.h"

void StandLight::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();

		m_spModel->Load("Asset/Data/Models/stand light/Light_Stand.gltf");

		Math::Matrix transMat = Math::Matrix::CreateTranslation(-50, -20, 70);
		Math::Matrix scaleMat = Math::Matrix::CreateScale(15.0f);
		m_mWorld = scaleMat * transMat;
	}
}

void StandLight::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
