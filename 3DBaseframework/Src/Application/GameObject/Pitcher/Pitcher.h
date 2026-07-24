#pragma once

// ===================================================
// ピッチャー表示用ポリゴン（回転体メッシュ）
//  DrawPolygon は TRIANGLESTRIP 専用。3D法線でライティングするため Set2DObject(false)。
// ===================================================
class PitcherBodyPolygon : public KdPolygon
{
public:
	PitcherBodyPolygon() { Set2DObject(false); }

	void SetVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }
};

// ===================================================
// ピッチャー（水を注ぐ器）
//  濃い茶色の不透明な回転体。コップの上に傾けて置く。
// ===================================================
class Pitcher : public KdGameObject
{
public:
	Pitcher()					{}
	~Pitcher()		override	{}

	void Init()		override;
	void DrawLit()	override;	// 不透明の茶色ジャグ

private:
	// プロファイルを Y軸回りに回転させてジャグの回転体メッシュ（側面＋底）を作る
	void BuildMesh();

	PitcherBodyPolygon m_poly;
};
