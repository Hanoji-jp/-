#pragma once
#include "../EffectBase/EffectBase.h"

class GameScene;

class RainbowBack : public EffectBase
{
public:
	void Init()							override;
	void Update()						override;
	void DrawSprite()					override;

	// アクティブ化　成功（良）時だけ表示する。初期は無効。
	void Activate();

private:
	// 拡縮の時間管理用
	float m_scaleTime = 0.0f;	

};