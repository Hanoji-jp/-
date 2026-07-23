#include "GameStart.h"
#include "GameStartConst.h"

namespace
{
	// 空文字でなければ音声を鳴らす（存在しないファイルを鳴らすとアサートで落ちるため、
	//  空パスは「鳴らさない」＝アセット未用意でも安全）。
	void PlayIfSet(const char* path)
	{
		if (path && path[0] != '\0')
		{
			KdAudioManager::Instance().Play(path);
		}
	}
}

void GameStart::Init()
{
	m_darkTex.Load(GameStartConst::kDarkImage);
	m_tex3.Load(GameStartConst::kCount3Image);
	m_tex2.Load(GameStartConst::kCount2Image);
	m_tex1.Load(GameStartConst::kCount1Image);
	m_startTex.Load(GameStartConst::kStartImage);

	// 3 から開始（3→2→1→start の順）
	m_phase = Phase::Count3;
	m_frame = 0;
	PlayIfSet(GameStartConst::kCount3Audio);
}

void GameStart::Restart()
{
	m_phase = Phase::Count3;
	m_frame = 0;
	PlayIfSet(GameStartConst::kCount3Audio);
}

// フェーズ切替＝そのフェーズの音声を頭から鳴らす
void GameStart::EnterPhase(Phase next)
{
	m_phase = next;
	m_frame = 0;

	switch (next)
	{
	case Phase::Count2:	PlayIfSet(GameStartConst::kCount2Audio);	break;
	case Phase::Count1:	PlayIfSet(GameStartConst::kCount1Audio);	break;
	case Phase::Start:	PlayIfSet(GameStartConst::kStartAudio);	break;	// 水をドンピシャで入れろ
	default:												break;
	}
}

void GameStart::Update()
{
	if (m_phase == Phase::Done) { return; }	// 演出終了後は何もしない

	++m_frame;

	switch (m_phase)
	{
	case Phase::Count3:
		if (m_frame >= GameStartConst::kCountFrames) { EnterPhase(Phase::Count2); }
		break;
	case Phase::Count2:
		if (m_frame >= GameStartConst::kCountFrames) { EnterPhase(Phase::Count1); }
		break;
	case Phase::Count1:
		if (m_frame >= GameStartConst::kCountFrames) { EnterPhase(Phase::Start); }
		break;
	case Phase::Start:
		if (m_frame >= GameStartConst::kStartFrames) { m_phase = Phase::Done; }	// 注水解禁
		break;
	default:
		break;
	}
}

void GameStart::DrawSprite()
{
	if (m_phase == Phase::Done) { return; }	// 終了後は何も描かない

	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

	// 全画面を少し暗く（画像に透過がある場合の背景。画像が全面不透明なら隠れる＝害なし）
	Math::Color dark(1.0f, 1.0f, 1.0f, GameStartConst::kDarkAlpha);
	sprite.DrawTex(&m_darkTex, 0, 0,
		GameStartConst::kOverlayWidth, GameStartConst::kOverlayHeight, nullptr, &dark);

	// 現在のフェーズの画像を全画面表示（3→2→1→start）
	KdTexture* tex = nullptr;
	switch (m_phase)
	{
	case Phase::Count3:	tex = &m_tex3;		break;
	case Phase::Count2:	tex = &m_tex2;		break;
	case Phase::Count1:	tex = &m_tex1;		break;
	case Phase::Start:	tex = &m_startTex;	break;
	default:									break;
	}
	if (tex)
	{
		sprite.DrawTex(tex, 0, 0, GameStartConst::kImageWidth, GameStartConst::kImageHeight);
	}

	KdShaderManager::Instance().UndoBlendState();
}
