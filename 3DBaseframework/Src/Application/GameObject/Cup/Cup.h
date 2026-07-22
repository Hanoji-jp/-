#pragma once

// 前方宣言
class KdTexture;

// ===================================================
// コップ
//  水を溜める器。目標ラインを保持し、2Dスプライトで描画する
// ===================================================
class Cup : public KdGameObject
{
public:
	Cup()						{}
	~Cup()			override	{}

	void Init()		override;
	void DrawLit()	override;

private:
	// コップ画像
	std::shared_ptr<KdTexture> m_spTex = nullptr;
};
