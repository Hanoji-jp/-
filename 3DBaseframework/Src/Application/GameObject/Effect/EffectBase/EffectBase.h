#pragma once

class EffectBase : public KdGameObject
{
public:

	EffectBase() { Init(); }
	~EffectBase() {}

	virtual void Init()							override;
	virtual void Update()						override {}
	virtual void DrawLit()						override {}
	virtual void DrawSprite()					override {}

	// 拡縮関数
	virtual void ScalingIteration(float _scaleMax, float _scaleMin, float _speed);


protected:
	//====================
	// 描画関係
	//====================
	// 板ポリゴン
	std::shared_ptr<KdSquarePolygon>	m_spPolygon = nullptr;

	// 2D画像
	std::shared_ptr<KdTexture> m_spTex = nullptr;

	// 行列 (拡大、回転、移動)
	Math::Matrix m_mScale = Math::Matrix::Identity;
	Math::Matrix m_mRotate = Math::Matrix::Identity;
	Math::Matrix m_mTrans = Math::Matrix::Identity;

	// 画像サイズ
	Math::Vector2 m_texSize = { 0.0f,0.0f };

	// 拡縮
	float	m_scale		= 1.0f;

	//====================
	// 移動関係
	//====================
	// 座標
	Math::Vector3 m_pos = { 0.0f,0.0f,0.0f };

	// 移動量
	Math::Vector3 m_move = { 0.0f,0.0f,0.0f };

	// 移動速度
	float m_speed = 0.05f;
};
