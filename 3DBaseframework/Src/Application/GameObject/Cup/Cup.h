#pragma once

// ===================================================
// コップのガラス表示用ポリゴン（回転体メッシュ）
//  DrawPolygon は TRIANGLESTRIP 専用。3D法線でライティングするため Set2DObject(false)。
// ===================================================
class CupGlassPolygon : public KdPolygon
{
public:
	CupGlassPolygon() { Set2DObject(false); }	// 2Dビルボード扱いにしない＝本物の3D

	// 頂点を差し替える（基底の protected 頂点配列へ書き込む）
	void SetVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }
};

// ===================================================
// コップ
//  水を溜める器。3Dの半透明ガラス（回転体メッシュ）と目標ラインを描画する。
// ===================================================
class Cup : public KdGameObject
{
public:
	Cup()						{}
	~Cup()			override	{}

	void Init()		override;
	void DrawLit()	override;	// 目標ライン
	void DrawEffect()override;	// 半透明ガラス（不透明シーンの後・水の直後に描く）

private:
	// kInnerShape の右半分を半径として Y軸回りに回転させ、ガラスの回転体メッシュを作る
	void BuildGlassMesh();

	// ガラス表示用メッシュ（三角形ストリップ）
	CupGlassPolygon m_glassPoly;
};
