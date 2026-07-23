#pragma once

// ===================================================
// ゲームオーバー表示（ラインに合わせられず終了したとき）
//  暗幕 →「終了」の落下 → リザルト明滅 を板ポリゴン(KdSquarePolygon)で表示する。
//  失敗（Under/Over）で終了したときだけ Activate() で有効化する。
//  ※GameScene が Activate()/Deactivate()/IsActive() を呼ぶ前提。
// ===================================================
class GameOver : public KdGameObject
{
public:

	GameOver() {}
	~GameOver() {}

	void Init()override;
	void Update()override;
	void DrawLit()override;

	// 表示の有効／無効（ライン不一致で終了したときだけ有効化）
	void Activate();
	void Deactivate() { m_active = false; }
	bool IsActive() const { return m_active; }

private:

	// 表示中か（false の間は Update / DrawLit とも何もしない）
	bool m_active = false;

	enum class Phase
	{
		TextFalling,	// 「終了」が降ってくる最中
		ShowResult		// 着地後、リザルトを明滅表示
	};
	Phase m_phase = Phase::TextFalling;

	// 板ポリゴン
	std::shared_ptr<KdSquarePolygon> m_polygon;		// 「終了」
	std::shared_ptr<KdSquarePolygon> m_result;		// リザルト（たいとる／やりなおし）
	std::shared_ptr<KdSquarePolygon> m_darkOverlay;	// 黒背景（暗幕）

	// 位置オフセット（ワールド）
	Math::Vector3 m_textOffset   = { 0.0f, 0.0f, 0.0f };	// 「終了」の現在位置
	Math::Vector3 m_resultOffset = { 0.0f, 0.0f, 0.0f };	// リザルトの表示位置

	int m_frame = 0;

	// 落下演出（ワールド単位。+Yが上）
	int   m_fallDuration = 45;		// 何フレームかけて降りるか
	float m_textStartY   = 2.0f;	// 降り始め（上）
	float m_textEndY     = 0.2f;	// 着地位置（中央やや上）
};
