#include "Clear.h"
#include"../../Scene/SceneManager.h"

void Clear::Init()
{}

void Clear::Update()
{
	m_Alpha += m_AlphaAdd;

	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, false);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ 0,0,0 }, m_Alpha);

	if (m_Alpha >= 1)
	{

		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);

	}

}