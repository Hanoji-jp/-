#pragma once

#include"../BaseScene/BaseScene.h"

// 前方宣言
class Water;
class GameOver;
class ClearEffectManager;
class GameStart;
class HandFinger;
class KdSoundInstance;

class GameScene : public BaseScene
{
public:

	GameScene() { Init(); }
	~GameScene() {}

private:

	void Event() override;
	void Init()  override;
	// DrawSprite は BaseScene の実装（全オブジェクトの DrawSprite を呼ぶ）をそのまま使うので
	// オーバーライドしない（以前の空オーバーライドで2Dが描かれなくなる事故を避ける）。

	// 注水を操作する対象の水
	std::weak_ptr<Water> m_wpWater;

	// 入場直後の押しっぱなしSPACEで即注水するのを防ぐ（一度離すまで注水しない）
	bool m_pourArmed = false;

	// 失敗時に表示するゲームオーバー
	std::weak_ptr<GameOver> m_wpGameOver;

	// 開始演出（水をドンピシャで入れろ→3・2・1→始めっ）。終わるまで注水を受け付けない。
	std::weak_ptr<GameStart> m_wpGameStart;

	// 指でボタンを押す手（注水中は押し込む）
	std::weak_ptr<HandFinger> m_wpHand;

	// 水が落ちている間だけループ再生する水音（water.mp3）。1本を使い回すので実体を保持する。
	std::shared_ptr<KdSoundInstance> m_waterSe;

	// 水音を鳴らしているか（再生／停止の切り替えを1回だけ行う）
	bool m_waterPlaying = false;

	// ボタン音（poti.mp3）。連打しても発声数を食い潰さないよう1つを使い回して鳴らし直す。
	std::shared_ptr<KdSoundInstance> m_potiSe;

	// SPACEの前フレーム押下状態（押した瞬間だけ鳴らす用）。
	//  ※関数ローカルstaticだとシーンを作り直しても状態が残るのでメンバで持つ。
	bool m_spaceDownPrev = false;

	// クリアエフェクトマネージャー
	std::weak_ptr<ClearEffectManager> m_wpClrEftMng;
};
