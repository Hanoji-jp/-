#pragma once

// ===================================================
// ゲーム開始演出（イントロ）
//  「水をドンピシャで入れろ！！！」→「3・2・1」→「始めっ！！！」を
//  音声＋画像で順に出し、終わったら注水を解禁する。
//  GameScene は IsFinished() が true になるまで注水を受け付けない。
// ===================================================
class GameStart : public KdGameObject
{
public:
	void Init()			override;
	void Update()		override;
	void DrawSprite()	override;

	// イントロが終わって操作可能になったか（GameScene が注水解禁の判定に使う）
	bool IsFinished() const { return m_phase == Phase::Done; }

	// 最初からやり直す（リトライ時など）
	void Restart();

private:
	// 演出の進行フェーズ（3→2→1→start の順）
	enum class Phase
	{
		Count3,			// 3
		Count2,			// 2
		Count1,			// 1
		Start,			// start.mp3（「水をドンピシャで入れろ！！！」＋画像）
		Done,			// 終了（注水解禁）
	};

	// フェーズを切り替え、そのフェーズの音声を鳴らす
	void EnterPhase(Phase next);

	Phase m_phase = Phase::Count3;
	int   m_frame = 0;	// 現フェーズ内の経過フレーム

	// 表示画像（各フェーズで全画面表示）
	KdTexture m_darkTex;		// 暗幕
	KdTexture m_tex3;		// 「3」
	KdTexture m_tex2;		// 「2」
	KdTexture m_tex1;		// 「1」
	KdTexture m_startTex;	// start（水をドンピシャで入れろ）
};
