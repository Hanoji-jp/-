#include "RyouEffect.h"

void RyouEffect::Init()
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

	//====================
	// エフェクト
	//====================
	// 挙動時間管理用
	m_time = 0;



	//乱数初期化
	srand(timeGetTime());

}

// 成功（良）で呼ぶ：演出を最初から再生する
//  ※GameScene は IsPourFinished()＝Done の間これを毎フレーム呼ぶため、
//    既に表示中なら何もしない。そうしないと毎フレーム拡縮がリセットされ止まる。
void RyouEffect::Activate()
{
	if (m_activeFlg) { return; }	// 既に表示中なら再スタートしない

	m_activeFlg = true;
	m_scale = 0.3f;		// 拡縮を初期値から
	m_scaleUp = true;	// 拡大方向から始める
	m_placeTime = kPlaceDuration;	// ポップイン演出を頭から再生（毎回リセット）

	// 音
	KdAudioManager::Instance().Play("Asset/Data/Audio/atari.wav",false);
}



void RyouEffect::Update()
{
	if (!m_activeFlg) { return; }	// 成功時だけ動く

	// タイマーカウント
	m_time++;

	// 良ボイス用カウント
	if (m_ryouVoiceCnt < m_ryouVoiceCntMax)
	{
		m_ryouVoiceCnt++;
	}
	else
	{
		m_ryouVoiceCnt = 0;
	}

	// アクティブになってから10秒経つまでブルブル
	if (m_time < 600.0f)
	{
		m_scale = 0.8f;
		// ブルブル
		Vibration(10.0f, -10.0f, 10.0f);
	}

	// アクティブになってから10秒後に拡縮開始
	if (m_time >= 600.0f)
	{
		// 拡縮反復
		ScalingIteration(2.0f, 0.3f, 0.7f);

		// 良ボイスランダム再生
		RyouVoice();
	}

	// 行列
	m_mScale = Math::Matrix::CreateScale(m_scale);
	m_mRotate = Math::Matrix::Identity;
	m_mTrans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = m_mScale * m_mRotate * m_mTrans;
}

void RyouEffect::DrawSprite()
{
	if (!m_activeFlg) { return; }	// 成功時だけ表示

	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, 0.0f, 0.0f);
	//KdDebugGUI::Instance().AddLog("RyouEffect DrawSprite");
	// 行列をリセット
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void RyouEffect::RyouVoice()
{
	// カウントが０でなければ再生しない
	if (m_playflg) return;

	KdAudioManager::Instance().Play("Asset/Data/Audio/ryou1.mp3", false);

	m_playflg = true;

	// ランダム
	//int i = rand() % 6 + 1;

	//switch (i)
	//{
	//case 1:
	//	KdAudioManager::Instance().Play("Asset/Data/Audio/ryou1.mp3", false);
	//	break;
	//case 2:
	//	KdAudioManager::Instance().Play("Asset/Data/Audio/ryou2.mp3", false);
	//	break;
	//case 3:
	//	KdAudioManager::Instance().Play("Asset/Data/Audio/ryou3.mp3", false);
	//	break;
	//case 4:
	//	KdAudioManager::Instance().Play("Asset/Data/Audio/ryou4.mp3", false);
	//	break;
	//case 5:
	//	KdAudioManager::Instance().Play("Asset/Data/Audio/ryou5.mp3", false);
	//	break;
	//case 6:
	//	KdAudioManager::Instance().Play("Asset/Data/Audio/ryou6.mp3", false);
	//	break;
	//default:
	//	break;

	//}
}
