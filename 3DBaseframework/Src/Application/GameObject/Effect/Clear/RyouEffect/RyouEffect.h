#pragma once
#include "../../EffectBase/EffectBase.h"

class GameScene;

class RyouEffect : public EffectBase
{
public:
	void Init()							override;
	void Update()						override;
	void DrawSprite()					override;

private:
	// 拡縮の時間管理用
	float m_scaleTime = 0.0f;	

};