#include "Clear.h"

void Clear::Init()
{
	m_Tex.Load("Asset/Textures/Clear(仮).png");
}

void Clear::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Tex, 0, 0, nullptr);
}
