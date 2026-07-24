#include "BlackoutEffect.h"

void BlackoutEffect::Init()
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
	for (int i = 0;i < TEX_NUM;i++)
	{
		// 実体化
		m_spTex[i] = std::make_shared<KdTexture>();

		// テクスチャの設定
		switch (i)
		{
		case 0:
			m_spTex[0]->Load("Asset/Textures/Effect/BlackoutEffect/blackout0.png");
			break;
		case 1:
			m_spTex[1]->Load("Asset/Textures/Effect/BlackoutEffect/blackout1.png");
			break;
		case 2:
			m_spTex[2]->Load("Asset/Textures/Effect/BlackoutEffect/blackout2.png");
			break;
		case 3:
			m_spTex[3]->Load("Asset/Textures/Effect/BlackoutEffect/blackout3.png");
			break;
		case 4:
			//m_spTex[4]->Load("Asset/Textures/Effect/RyouEffect/ryou.png");
			break;
		default:
			break;
		}
		
		// 行列
		m_mWorld[i] = Math::Matrix::Identity;

		// 色
		m_color[i] = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// 行列 (拡大、回転、移動)
	m_mTrans = Math::Matrix::Identity;
	m_mRotate = Math::Matrix::Identity;
	m_mScale = Math::Matrix::Identity;

	// 画像サイズ
	m_texSize = { 192.0f,128.0f };

	// 拡縮
	m_scale = 1.0f;

	//====================
	// 移動関係
	//====================
	// 座標
	m_pos = { 0.0f,0.0f,0.0f };

	//====================
	// 挙動関係
	//====================
	// エフェクトの時間
	m_time = 0;

}

void BlackoutEffect::Activate()
{
	if (m_activeFlg && !m_endFlg) { return; }	// 既に表示中なら再スタートしない

	m_activeFlg = true;
	m_scale = 1.0f;		// 拡縮を初期値から
	m_scaleUp = true;	// 拡大方向から始める
	// 音
	KdAudioManager::Instance().Play("Asset/Data/Audio/freeze.wav", false);
}

void BlackoutEffect::Update()
{
	if (!m_activeFlg && !m_endFlg) { return; }
	
	//画像1（白い光）の処理
	{
		// 画像1用負透明度
		static float _bo1Alp = 0.5f;

		// 不透明度に加算する値
		static const float _bo1AlpInterval = 0.05;

		if (m_time < 10)
		{
			_bo1Alp += _bo1AlpInterval;
		}
		else if (m_time >= 10 && m_time < 30)
		{
			_bo1Alp -= _bo1AlpInterval;
		}
		else if (m_time == 49)
		{
			_bo1Alp = 0.5f;
		}
		else if (m_time >= 50 && m_time < 60)
		{
			_bo1Alp += _bo1AlpInterval;
		}
		else if (m_time >= 60 && m_time < 90)
		{
			//_bo1Alp -= _bo1AlpInterval;
		}
		m_color[1] = { 1.0f, 1.0f, 1.0f, _bo1Alp };
	}
	
	//画像2の処理
	{
		// 画像3の大きさ
		static Math::Vector2 _bo2Scale = { 1.0f,1.0f };

		if (m_time < 5)
		{
			_bo2Scale.y += 0.05f;
		}
		else if (m_time >= 10 && m_time < 23)
		{
			_bo2Scale.y -= 0.1f;
		}
		else
		{
			m_color[2] = { 1.0f, 1.0f, 1.0f, 0.0f };
		}


		m_mWorld[2] = Math::Matrix::CreateScale(_bo2Scale.x, _bo2Scale.y, 1.0f);
	}

	//画像3の処理
	{
		// 画像3の大きさ
		static Math::Vector2 _bo3Scale = { 1.0f,1.0f };

		if ( m_time < 5)
		{
			_bo3Scale.y += 0.05f;
		}
		else if (m_time >= 10 && m_time < 23)
		{
			_bo3Scale.y -= 0.1f;
		}
		else
		{
			m_color[3] = { 1.0f, 1.0f, 1.0f, 0.0f };
		}

		m_mWorld[3] = Math::Matrix::CreateScale(_bo3Scale.x, _bo3Scale.y, 1.0f);
	}

	//画像4の処理
	{
		//// 画像3の大きさ
		//static Math::Vector2 _bo4Scale = { 0.0f,0.0f };

		//// 不透明度に加算する値
		//static const Math::Vector2 _bo4ScaleInterval = { 0.15f,0.15f };

		//if (m_time == 30 )
		//{
		//	_bo4Scale = { 2.0f,2.0f };
		//}
		//else if (m_time > 30 && m_time < 40)
		//{
		//	_bo4Scale -= _bo4ScaleInterval;
		//}
		//else if (m_time >= 40 && m_time < 45)
		//{
		//	_bo4Scale += _bo4ScaleInterval;
		//}
		//else if (m_time >= 45 && m_time < 50)
		//{
		//	_bo4Scale -= _bo4ScaleInterval;
		//}
		//else if (m_time >= 50 && m_time < 55)
		//{
		//	_bo4Scale += _bo4ScaleInterval;
		//}
		//else if (m_time >= 55 && m_time < 60)
		//{
		//	_bo4Scale -= _bo4ScaleInterval;
		//}

		//m_mWorld[4] = Math::Matrix::CreateScale(_bo4Scale.x, _bo4Scale.y, 1.0f);
	}

	// エフェクトの時間カウント
	m_time++;

	// 終了フラグ
	if (m_time >= 60)  m_endFlg = true;

	

	// 行列
	m_mScale = Math::Matrix::CreateScale(m_scale);
	m_mRotate = Math::Matrix::Identity;
	m_mTrans = Math::Matrix::CreateTranslation(m_pos);
	//m_mWorld = m_mScale * m_mRotate * m_mTrans;
}

void BlackoutEffect::DrawSprite()
{
	if (!m_activeFlg) { return; }

	for (int i = 0;i < TEX_NUM;i++)
	{
		KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld[i]);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex[i], 0.0f, 0.0f, nullptr, &m_color[i]);
	}

	//KdDebugGUI::Instance().AddLog("BlackoutEffect DrawSprite");

	// 行列をリセット
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}
