#include "RyouEffect.h"

void RyouEffect::Init()
{
	//====================
	// 描画関係
	//====================
	// 実体化
	m_spTex = std::make_shared<KdTexture>();

	// テクスチャの設定
	m_spTex->Load("Asset/Textures/Effect/RyouEffect/ryou.png");

	// 行列 (拡大、回転、移動)
	m_mTrans = Math::Matrix::Identity;
	m_mRotate = Math::Matrix::Identity;
	m_mScale = Math::Matrix::Identity;

	// 画像サイズ
	m_texSize = { 192.0f,128.0f };

	// 拡縮
	m_scale = 0.3f;

	//====================
	// 移動関係
	//====================
	// 座標
	m_pos = { 0.0f,0.0f,0.0f };

}

// 成功（良）で呼ぶ：演出を最初から再生する
void RyouEffect::Activate()
{
	m_active = true;
	m_scale = 0.3f;	// 拡縮を初期値から
}

void RyouEffect::Update()
{
	if (!m_active) { return; }	// 成功時だけ動く

	// 拡縮反復
	ScalingIteration(2.0f,0.3f,0.8f);

	// 行列
	m_mScale = Math::Matrix::CreateScale(m_scale);
	m_mRotate = Math::Matrix::Identity;
	m_mTrans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = m_mScale * m_mRotate * m_mTrans;
}

void RyouEffect::DrawSprite()
{
	if (!m_active) { return; }	// 成功時だけ表示

	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, 0.0f, 0.0f);
	KdDebugGUI::Instance().AddLog("RyouEffect DrawSprite");
	// 行列をリセット
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}
