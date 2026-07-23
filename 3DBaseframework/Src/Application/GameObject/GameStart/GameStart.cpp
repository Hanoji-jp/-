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
	m_instrTex.Load(GameStartConst::kInstrImage);
	m_goTex.Load(GameStartConst::kGoImage);

	// 最初のフェーズ（水をドンピシャで入れろ）から開始
	m_phase = Phase::Instruction;
	m_frame = 0;
	PlayIfSet(GameStartConst::kInstrAudio);
}

void GameStart::Restart()
{
	m_phase = Phase::Instruction;
	m_frame = 0;
	PlayIfSet(GameStartConst::kInstrAudio);
}

// フェーズ切替＝そのフェーズの音声を頭から鳴らす
void GameStart::EnterPhase(Phase next)
{
	m_phase = next;
	m_frame = 0;

	switch (next)
	{
	case Phase::Count3:	PlayIfSet(GameStartConst::kCount3Audio);	break;
	case Phase::Count2:	PlayIfSet(GameStartConst::kCount2Audio);	break;
	case Phase::Count1:	PlayIfSet(GameStartConst::kCount1Audio);	break;
	case Phase::Go:		PlayIfSet(GameStartConst::kGoAudio);		break;
	default:												break;
	}
}

void GameStart::Update()
{
	if (m_phase == Phase::Done) { return; }	// 演出終了後は何もしない

	++m_frame;

	switch (m_phase)
	{
	case Phase::Instruction:
		if (m_frame >= GameStartConst::kInstrFrames) { EnterPhase(Phase::Count3); }
		break;
	case Phase::Count3:
		if (m_frame >= GameStartConst::kCountFrames) { EnterPhase(Phase::Count2); }
		break;
	case Phase::Count2:
		if (m_frame >= GameStartConst::kCountFrames) { EnterPhase(Phase::Count1); }
		break;
	case Phase::Count1:
		if (m_frame >= GameStartConst::kCountFrames) { EnterPhase(Phase::Go); }
		break;
	case Phase::Go:
		if (m_frame >= GameStartConst::kGoFrames) { m_phase = Phase::Done; }	// 注水解禁
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

	// 全画面を少し暗くして文字を見やすく
	Math::Color dark(1.0f, 1.0f, 1.0f, GameStartConst::kDarkAlpha);
	sprite.DrawTex(&m_darkTex, 0, 0,
		GameStartConst::kOverlayWidth, GameStartConst::kOverlayHeight, nullptr, &dark);

	// フェーズごとの画像（カウント中は数字画像が無いので音声のみ＝暗幕だけ）
	if (m_phase == Phase::Instruction)
	{
		sprite.DrawTex(&m_instrTex, 0, 0,
			GameStartConst::kInstrWidth, GameStartConst::kInstrHeight);
	}
	else if (m_phase == Phase::Go)
	{
		sprite.DrawTex(&m_goTex, 0, 0,
			GameStartConst::kGoWidth, GameStartConst::kGoHeight);
	}

	KdShaderManager::Instance().UndoBlendState();
}
