#pragma once
#include "../../EffectBase/EffectBase.h"

class GameScene;

class RyouEffect : public EffectBase
{
public:
	void Init()							override;
	void Update()						override;
	void DrawSprite()					override;

	// アクティブ化　成功（良）時だけ表示する。初期は無効。
	void Activate();

private:

};