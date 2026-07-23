#pragma once

// ===================================================
// ゲームオーバー表示（ラインに合わせられず終了したとき）
//  全画面2Dスプライトで、暗幕 →「終了」の落下 → リザルト明滅 を表示する。
//  2Dスプライト(DrawSprite)は全描画パスの最後・深度無しで走るため、
//  シーンの3D物や描画順に関係なく画面全体を確実に覆える。
//  ※GameScene::DrawSprite() が BaseScene::DrawSprite() を呼ぶ前提。
//  失敗（Under/Over）で終了したときだけ Activate() で有効化する。
// ===================================================
class GameOver : public KdGameObject
{
public:

	GameOver() {}
	~GameOver() {}

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 表示の有効／無効（ライン不一致で終了したときだけ有効化）
	void Activate();
	void Deactivate() { m_active = false; }
	bool IsActive() const { return m_active; }

private:

	// 表示中か（false の間は Update / DrawSprite とも何もしない）
	bool m_active = false;

	enum class Phase
	{
		TextFalling,	// 「終了」が降ってくる最中
		ShowResult		// 着地後、リザルトを明滅表示
	};
	Phase m_phase = Phase::TextFalling;

	// 2D画像
	KdTexture m_darkTex;	// 黒の暗幕（kuro.png を全画面に貼る）
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

	// 画像の表示サイズ（ピクセル）
	static constexpr int kGoWidth     = 460;
	static constexpr int kGoHeight    = 200;
	static constexpr int kResultWidth = 520;
	static constexpr int kResultHeight = 180;
	// 暗幕の表示サイズ（画面より十分大きく。中心原点なので全画面を覆う）
	static constexpr int kOverlayWidth  = 2400;
	static constexpr int kOverlayHeight = 1600;
};
