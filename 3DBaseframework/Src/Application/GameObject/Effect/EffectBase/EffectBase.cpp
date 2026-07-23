#include "EffectBase.h"

void EffectBase::Init()
{
	//====================
	// 描画関係
	//====================
	// 行列 (拡大、回転、移動)
	m_mScale = Math::Matrix::Identity;
	m_mRotate = Math::Matrix::Identity;
	m_mTrans = Math::Matrix::Identity;

	// 画像サイズ
	m_texSize = { 0.0f,0.0f };

	// 拡縮
	m_scale = 1.0f;

	//====================
	// 移動関係
	//====================
	// 座標
	m_pos = { 0.0f,0.0f,0.0f };

	// 移動量
	m_move = { 0.0f,0.0f,0.0f };

	// 移動速度
	m_speed = 0.05f;
}


void EffectBase::ScalingIteration(float _scaleMax, float _scaleMin, float _speed)
{
	if (m_scaleUp)
	{
		m_scale += _speed;
	}
	else
	{
		m_scale -= _speed;
	}

	if (m_scale >= _scaleMax)
	{
		m_scaleUp = false;
	}
	else if (m_scale < _scaleMin)
	{
		m_scaleUp = true;
	}
}
