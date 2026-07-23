#include "Window.h"

void Window::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();

		m_spModel->Load("Asset/Data/Models/window/window.gltf");

		Math::Matrix transMat = Math::Matrix::CreateTranslation(50, -5, 70);
		Math::Matrix scaleMat = Math::Matrix::CreateScale(30.0f);
		Math::Matrix rotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(0));
		m_mWorld = scaleMat * rotYMat * transMat;
	}
}

void Window::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
