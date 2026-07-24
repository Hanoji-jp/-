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

	// 良ボイス
	void RyouVoice();

private:
	// 良ボイス用変数
	int m_ryouVoiceCnt = 0;
	static const int m_ryouVoiceCntMax = 120;
	bool m_playflg = false;
};