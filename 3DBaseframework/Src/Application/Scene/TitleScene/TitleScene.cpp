#include "TitleScene.h"
#include "../SceneManager.h"

void TitleScene::Draw()
{
	Math::Color color = { 1.0f,1.0f,1.0f,1.0f };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 640, 360, &color, true);

	/*Math::Color enterColor = { 1.0f,1.0f,1.0f,m_alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex
	(
		&Tex, 0, -180, 500, 140, &rc, &enterColor
	);*/
}

void TitleScene::Update()
{
	m_alpha += m_alphaAdd;

	if (m_alpha > 1.0f)
	{
		m_alpha = 1.0f;
		m_alphaAdd *= -1;
	}
	else if (m_alpha < 0.4f)
	{
		m_alpha = 0.4f;
		m_alphaAdd *= -1;
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}

void TitleScene::Event()
{
	
}

void TitleScene::Init()
{}
