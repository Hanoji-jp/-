#pragma once

#include"../BaseScene/BaseScene.h"

class TitleScene : public BaseScene
{
public:

	TitleScene() { Init(); }
	~TitleScene() { KdAudioManager::Instance().StopAllSound(); }

	void Draw()override;
	void Update() override;

private:

	void Event() override;
	void Init()  override;

	//プレスキーの点滅用変数
	float m_alphaAdd = 0.01f;
	float m_alpha = 1.0f;

	//画像テクスチャ
	KdTexture m_spaceTex;
	KdTexture m_titleTex;
	KdTexture m_waveTex;

	//波画像のスクロールとサインカーブ用の変数
	float m_waveScrollX = 0.0f;
	float m_waveTime = 0.0f;
};
