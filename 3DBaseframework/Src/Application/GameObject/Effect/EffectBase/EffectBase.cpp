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
	// ポップイン演出：起動時に大きく出て、目標スケールへ縮んで止まる。
	//  ※static を使わずメンバ m_placeTime で管理（インスタンスごと・Activateで再生し直せる）。
	const float kPop    = 2.2f;		// 出た瞬間の大きさ
	const float kTarget = 1.4f;		// 落ち着く大きさ（この大きさで「良」を表示）

	if (m_placeTime > 0)
	{
		const float t = static_cast<float>(m_placeTime) / static_cast<float>(kPlaceDuration);	// 1→0
		m_scale = kTarget + (kPop - kTarget) * t;	// pop→target へイーズ
		--m_placeTime;
	}
	else
	{
		m_scale = kTarget;	// 目標サイズで保持
	}
}
