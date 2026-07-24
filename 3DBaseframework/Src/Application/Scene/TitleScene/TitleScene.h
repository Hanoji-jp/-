#pragma once

#include"../BaseScene/BaseScene.h"

// 前方宣言
class FluidField;

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

	//背景の流体（サイン波画像の代わりに、実際のGPU流体シミュレーションを表示する）
	std::shared_ptr<FluidField> m_fluid;

	//流体を活かしておくための注水タイマー（一定周期で少し注いで波立たせる）
	float m_fluidTime = 0.0f;
};
