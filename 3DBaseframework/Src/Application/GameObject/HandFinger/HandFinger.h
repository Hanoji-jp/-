#pragma once

// ===================================================
// 手の画像を貼る3D板ポリゴン
//  KdPolygon の頂点配列を差し替えるため subclass 化する（Water と同じ方式）。
// ===================================================
class HandPolygon : public KdPolygon
{
public:
	HandPolygon() { Set2DObject(false); }	// 3D空間の板ポリとして扱う

	void SetVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }
};

// ===================================================
// 指でボタンを押す手（3Dの板ポリゴンで描画）
//  注水中（SPACE押下中）は指が押し込まれる方向へ動き、離すと戻る。
//  押下状態は GameScene が SetPressed() で毎フレーム渡す。
// ===================================================
class HandFinger : public KdGameObject
{
public:
	void Init()			override;
	void Update()		override;
	void DrawEffect()	override;	// 半透明なので不透明シーンの後に描く

	// 押しているか（＝注水中か）を毎フレーム設定する
	void SetPressed(bool pressed) { m_pressed = pressed; }

private:
	HandPolygon m_poly;						// 手の板ポリ（1x1の単位クアッド）
	std::shared_ptr<KdTexture> m_spTex;		// 手の画像

	bool  m_pressed = false;	// 押下入力
	float m_press   = 0.0f;		// 押し込み量 0=離す〜1=押す（滑らかに追従）
};
