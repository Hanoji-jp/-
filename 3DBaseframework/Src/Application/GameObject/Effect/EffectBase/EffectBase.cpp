#include "EffectBase.h"

void EffectBase::Init()
{
	//====================
	// 実行用フラグなど
	//====================
	// アクティブ状態フラグ
	m_activeFlg = false;

	// エフェクト終了フラグ
	m_endFlg = false;

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

void EffectBase::PlaceChara()
{
	static int _time = 30;
	if (_time > 0)
	{
		_time--;
	}

	if (_time == 30)
	{
		m_scale = 5.0f;
	}
	else if (_time < 30 && _time >= 25)
	{
		m_scale += 0.02f;
	}
	else if (_time < 25 && _time >= 5)
	{
		m_scale -= 0.05f;
	}
	else if (_time < 5 && _time > 0)
	{
		m_scale -= 0.01f;
	}
	else
	{
		//m_scale = 1.0f;
	}

}
