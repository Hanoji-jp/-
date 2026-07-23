#include "DrinkBar.h"
#include "DrinkBarConst.h"

void DrinkBar::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Data/Models/DrinkBar/DrinkBar.gltf");
	}

	m_drawType = eDrawTypeLit;
}

void DrinkBar::DrawLit()
{
	if (!m_spModel) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, DrinkBarConst::WorldMatrix());
}
