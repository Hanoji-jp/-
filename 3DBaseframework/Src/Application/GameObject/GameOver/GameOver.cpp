#include "GameOver.h"

void GameOver::Init()
{
	// 「終了」とリザルトの画像を読み込む
	m_goTex.Load("Asset/Textures/gameover.png");
	m_resultTex.Load("Asset/Textures/resultUI.png");

	m_phase = Phase::TextFalling;
	m_frame = 0;
	m_textY = m_textStartY;
	m_resultAlpha = 0.0f;
}

// ライン不一致で終了したときに呼ぶ：演出を最初から再生する
void GameOver::Activate()
{
	m_active = true;
	m_phase = Phase::TextFalling;
	m_frame = 0;
	m_textY = m_textStartY;
	m_resultAlpha = 0.0f;
}

void GameOver::Update()
{
	if (!m_active) { return; }	// 失敗で終了したときだけ動く

	m_frame++;

	if (m_phase == Phase::TextFalling)
	{
		float t = std::min(1.0f, (float)m_frame / (float)m_fallDuration);
		float easedT = 1.0f - std::powf(1.0f - t, 3.0f);	// だんだん減速して着地

		m_textY = m_textStartY + (m_textEndY - m_textStartY) * easedT;

		// 落下完了 → リザルト表示フェーズへ
		if (t >= 1.0f)
		{
			m_phase = Phase::ShowResult;
			m_frame = 0;
		}
	}
	else // ShowResult
	{
		m_textY = m_textEndY;

		// リザルトの明滅
		float t = (std::sinf(m_frame * 0.05f) + 1.0f) * 0.5f;
		m_resultAlpha = 0.2f + 0.7f * t;
	}
}

void GameOver::DrawSprite()
{
	if (!m_active) { return; }	// 失敗で終了したときだけ表示

	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// 暗幕（全画面を暗くする）。DrawBox の extent は「半分の大きさ」なので画面より広めに
	const Math::Color darkColor = { 0.0f, 0.0f, 0.0f, 0.6f };
	sprite.DrawBox(0, 0, 800, 500, &darkColor, true);

	// 「終了」（落下中／着地）。サイズは実寸ではなく明示指定
	const Math::Color white = { 1.0f, 1.0f, 1.0f, 1.0f };
	sprite.DrawTex(&m_goTex, 0, (int)m_textY, 460, 200, nullptr, &white);

	// リザルト（着地後に明滅）
	if (m_phase == Phase::ShowResult)
	{
		const Math::Color resultColor = { 1.0f, 1.0f, 1.0f, m_resultAlpha };
		sprite.DrawTex(&m_resultTex, 0, (int)m_resultY, 520, 180, nullptr, &resultColor);
	}
}
