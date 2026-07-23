#pragma once

// ===================================================
// ゲームオーバー表示（ラインに合わせられず終了したとき）
//  全画面2Dスプライトで、暗幕→「終了」の落下→リザルト明滅 を表示する。
//  ※ワールドの板ポリだとカメラ（コップ）に縛られて縮むため、スプライトで描く。
//    表示には GameScene::DrawSprite() が BaseScene::DrawSprite() を呼ぶ必要がある。
// ===================================================
class GameOver : public KdGameObject
{
public:

	GameOver() {}
	~GameOver() {}

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 表示の有効／無効（ライン不一致で終了したときだけ有効化する）
	void Activate();
	void Deactivate() { m_active = false; }
	bool IsActive() const { return m_active; }

private:

	// 表示中かどうか（false のときは Update も DrawSprite も何もしない）
	bool m_active = false;

	enum class Phase
	{
		TextFalling, // 「終了」の文字が降ってくる最中
		ShowResult   // 着地後、リザルトを明滅表示
	};
	Phase m_phase = Phase::TextFalling;

	// 2D画像
	KdTexture m_goTex;		// 「終了」
	KdTexture m_resultTex;	// リザルト（たいとる／やりなおし）

	int   m_frame = 0;
	float m_textY = 0.0f;		// 「終了」の現在Y（ピクセル。+上）
	float m_resultAlpha = 0.0f;	// リザルトの明滅アルファ

	// 落下演出パラメータ（ピクセル。+Yが上）
	int   m_fallDuration = 45;		// 何フレームかけて降りるか
	float m_textStartY   = 320.0f;	// 降り始め（画面上）
	float m_textEndY     = 70.0f;	// 着地位置（中央やや上）
	float m_resultY      = -150.0f;	// リザルトの表示位置（中央やや下）
};
