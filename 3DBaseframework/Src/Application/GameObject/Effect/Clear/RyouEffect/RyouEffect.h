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

};