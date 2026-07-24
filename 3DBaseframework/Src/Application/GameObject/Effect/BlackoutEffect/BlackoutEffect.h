#pragma once
#include "../EffectBase/EffectBase.h"

class GameScene;

class BlackoutEffect : public EffectBase
{
public:
	void Init()							override;
	void Update()						override;
	void DrawSprite()					override;

	// アクティブ化　成功（良）時だけ表示する。初期は無効。
	void Activate();

private:
	static const int TEX_NUM = 5;

	// 2D画像
	std::shared_ptr<KdTexture> m_spTex[TEX_NUM];

	// 3D空間に存在する機能
	Math::Matrix	m_mWorld[TEX_NUM];

	// 色
	Math::Color m_color[TEX_NUM];

};