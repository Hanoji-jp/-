#pragma once

#include"../BaseScene/BaseScene.h"

class TitleScene : public BaseScene
{
public:

	TitleScene() { Init(); }
	~TitleScene() {}

	void Draw()override;
	void Update() override;

private:

	void Event() override;
	void Init()  override;

	float m_alphaAdd = 0.01f;
	float m_alpha = 1.0f;

	KdTexture m_spaceTex;
	KdTexture m_titleTex;
	KdTexture m_waveTex;

	float m_waveScrollX = 0.0f;
};
