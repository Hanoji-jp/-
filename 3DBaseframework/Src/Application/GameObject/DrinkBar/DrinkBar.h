#pragma once

// ===================================================
// ドリンクバー（ディスペンサー）
//  gltfモデルを読み込んで表示する。ノズルの下にコップを置いて水を注ぐ。
// ===================================================
class DrinkBar : public KdGameObject
{
public:

	DrinkBar()					{}
	~DrinkBar()		override	{}

	void Init()		override;
	void DrawLit()	override;

private:

	std::shared_ptr<KdModelData> m_spModel;
};
