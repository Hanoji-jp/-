#pragma once
#include "../../EffectBase/EffectBase.h"

class GameScene;

class RyouEffect : public EffectBase
{
public:
	void Init()							override;
	void Update()						override;
	void DrawSprite()					override;

	// 成功（良）時だけ表示する。初期は無効。
	void Activate();
	void Deactivate() { m_active = false; }
	bool IsActive() const { return m_active; }

private:

	// 表示中かどうか（false のとき Update も DrawSprite も何もしない）
	bool m_active = false;
};