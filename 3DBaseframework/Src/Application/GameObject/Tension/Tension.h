#pragma once

// 前方宣言
class CameraBase;
class Water;
class GameStart;
class KdSoundInstance;

// ===================================================
// 緊張演出（心臓音＋コップへズーム＋上下の黒帯を縮める）
//  水位が目標ラインに近づくほど強くなる：
//   ・心臓音を「普通→籠り」へ音量クロスフェード（両方ループ再生）
//   ・カメラをコップへ少しずつズームイン
//   ・上下の黒帯を少しずつ縮める
// ===================================================
class Tension : public KdGameObject
{
public:
	void Init()			override;
	void Update()		override;
	void DrawSprite()	override;

	// 演出対象を渡す（GameScene が生成後に呼ぶ）
	void SetCamera(const std::shared_ptr<CameraBase>& cam) { m_wpCamera = cam; }
	void SetWater(const std::shared_ptr<Water>& water)     { m_wpWater = water; }
	void SetGameStart(const std::shared_ptr<GameStart>& gs){ m_wpGameStart = gs; }

private:
	std::weak_ptr<CameraBase> m_wpCamera;
	std::weak_ptr<Water>      m_wpWater;
	std::weak_ptr<GameStart>  m_wpGameStart;	// 開始演出（終わったら心臓音を鳴らす）

	// 開始演出が終わって心臓音を鳴らし始めたか
	bool m_started = false;

	// 心臓音の再生インスタンス（普通・籠り。両方ループし音量で切替）
	std::shared_ptr<KdSoundInstance> m_heart;
	std::shared_ptr<KdSoundInstance> m_muffled;

	// 上下の黒帯用テクスチャ
	KdTexture m_blackTex;

	// 演出の強さ（0=遠い〜1=目標付近）。滑らかに追従させる。
	float m_intensity = 0.0f;
};
