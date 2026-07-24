#pragma once
#include "../EffectBase/EffectBase.h"

// 各エフェクト前方宣言
class GameScene;
class RyouEffect;
class BlackoutEffect;

class ClearEffectManager : public EffectBase
{
public:
	void Init() override;
	void Update() override;

	// エフェクト実体化
	void EffectMaterial();

	// 非アクティブ化
	void Deactivate() override;

	// シーン情報をセット
	void SetOwner(GameScene* _owner) { m_owner = _owner; }

private:
	// ゲームシーンアドレス格納
	GameScene* m_owner = nullptr;

	// クリアしてからの時間（このオブジェクトがアクティブ化してからの時間）
	int m_ClearTime = 0;

	// 各エフェクトポインタ宣言
	std::shared_ptr<RyouEffect> m_spRyou;
	
	std::shared_ptr<BlackoutEffect> m_spBlackout;
	
};