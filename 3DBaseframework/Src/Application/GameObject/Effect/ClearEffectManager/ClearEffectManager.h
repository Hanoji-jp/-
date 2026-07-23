#pragma once
#include "../EffectBase/EffectBase.h"

class GameScene;
class RyouEffect;

class ClearEffectManager : public EffectBase
{
public:
	void Init() override;
	void Update() override;

private:
	// クリアしてからの時間（このオブジェクトが呼び出されてからの時間）
	int m_ClearTime;
};