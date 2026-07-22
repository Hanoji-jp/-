#include "TitleScene.h"
#include "../SceneManager.h"

void TitleScene::Draw()
{
	Math::Color color = { 1.0f,1.0f,1.0f,1.0f };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 640, 360, &color, true);

	int waveOffsetY = (int)(sinf(m_waveTime) * 100.0f);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_waveTex, (int)m_waveScrollX, waveOffsetY - 100, 1634, 963);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_waveTex, (int)m_waveScrollX + 1634, waveOffsetY - 100, 1634, 963);

	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleTex, 0, 100, 512, 256);

	Math::Rectangle rc = { 0, 0, 512, 256 };
	Math::Color spaceColor = { 1.0f,1.0f,1.0f,m_alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex
	(
		&m_spaceTex, 0, -180, 512, 256, &rc, &spaceColor
	);
}

void TitleScene::Update()
{

	// スクロール位置の更新
	m_waveScrollX -= 2.0f;

	m_waveTime += 0.03f;

	// ループ処理
	if (m_waveScrollX <= -1634.0f) m_waveScrollX += 1634.0f;

	m_alpha += m_alphaAdd;

	if (m_alpha > 1.0f)
	{
		m_alpha = 1.0f;
		m_alphaAdd *= -1;
	}
	else if (m_alpha < 0.2f)
	{
		m_alpha = 0.2f;
		m_alphaAdd *= -1;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
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
{
	m_spaceTex.Load("Asset/Textures/space.png");
	m_titleTex.Load("Asset/Textures/title_kari.png");
	m_waveTex.Load("Asset/Textures/wave3.png");
}
