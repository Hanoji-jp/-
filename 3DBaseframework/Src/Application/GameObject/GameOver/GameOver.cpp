#include "GameOver.h"

void GameOver::Init()
{
	// 暗幕・「終了」・リザルトの画像を読み込む
	m_darkTex.Load("Asset/Textures/kuro.png");
	m_goTex.Load("Asset/Textures/gameover.png");
	m_resultTex.Load("Asset/Textures/resultUI.png");

	m_phase = Phase::TextFalling;
	m_frame = 0;
	m_textY = m_textStartY;
	m_resultAlpha = 0.0f;
}

// 失敗で終了したときに呼ぶ（落下演出を頭から再生する）
//  ※GameScene は IsPourFinished()＝Done の間これを毎フレーム呼ぶため、
//    既に表示中なら何もしない。そうしないと毎フレーム frame=0 にリセットされ
//    落下アニメが進まない（特にリトライ後に顕著）。
void GameOver::Activate()
{
	if (m_active) { return; }	// 既に表示中なら再スタートしない

	m_active = true;
	m_phase  = Phase::TextFalling;
	m_frame  = 0;
	m_textY  = m_textStartY;
	m_resultAlpha = 0.0f;

	KdAudioManager::Instance().Play("Asset/Data/Audio/failed.WAV", false);
}

void GameOver::Update()
{
	// 無効の間は何もしない（失敗時のみ表示）
	if (!m_active) { return; }

	m_frame++;

	if (m_phase == Phase::TextFalling)
	{
		float t = std::min(1.0f, (float)m_frame / (float)m_fallDuration);

		// イージング（速く落ちて、ゆっくり止まる）
		float easedT = 1.0f - std::powf(1.0f - t, 3.0f);

		m_textY = m_textStartY + (m_textEndY - m_textStartY) * easedT;

		// 落下完了 → リザルト表示フェーズへ
		if (t >= 1.0f)
		{
			m_phase = Phase::ShowResult;
			m_frame = 0;
		}
	}
	else if (m_phase == Phase::ShowResult)
	{
		// リザルトの明滅（0.2～0.9 を行き来）
		float s = (std::sinf(m_frame * 0.05f) + 1.0f) * 0.5f;
		m_resultAlpha = 0.2f + 0.7f * s;
	}
}

void GameOver::DrawSprite()
{
	// 無効の間は描かない（失敗時のみ表示）
	if (!m_active) { return; }

	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// 透過合成（暗幕・画像のアルファを効かせる）
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

	// 黒の暗幕（kuro.png を全画面に貼る。中心原点・+Y上のピクセル座標）
	//  DrawTex は実績のあるパス。色(1,1,1,α)で不透明度だけ与える。
	Math::Color dark(1.0f, 1.0f, 1.0f, 0.6f);
	sprite.DrawTex(&m_darkTex, 0, 0, kOverlayWidth, kOverlayHeight, nullptr, &dark);

	// 「終了」の文字（上から落ちてくる）
	sprite.DrawTex(&m_goTex, 0, (int)m_textY, kGoWidth, kGoHeight);

	// リザルト（着地後に明滅表示）
	if (m_phase == Phase::ShowResult)
	{
		Math::Color col(1.0f, 1.0f, 1.0f, m_resultAlpha);
		sprite.DrawTex(&m_resultTex, 0, (int)m_resultY, kResultWidth, kResultHeight, nullptr, &col);
	}

	KdShaderManager::Instance().UndoBlendState();
}
