#pragma once

// ===================================================
// コップ
//  水を溜める器。枠と目標ラインを板ポリゴン（ライン）で描画する
// ===================================================
class Cup : public KdGameObject
{
public:
	Cup()						{}
	~Cup()			override	{}

	void Init()		override;
	void DrawLit()	override;
};
