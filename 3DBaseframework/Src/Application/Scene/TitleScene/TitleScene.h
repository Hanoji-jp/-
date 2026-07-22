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


};
