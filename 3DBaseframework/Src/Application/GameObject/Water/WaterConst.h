#pragma once

// ===================================================
// 水（簡易2Dグリッド流体：1次元高さ場／浅水近似）関連の定数
// ===================================================
namespace WaterConst
{
	// 水面を左右に分割する列（グリッド）の数
	inline constexpr int kColumnCount = 60;

	// 波の伝播の強さ（隣の列へ高さ差が伝わる強さ）
	inline constexpr float kWaveSpread = 8.0f;

	// 速度の減衰（1秒あたり）：波を徐々に収める
	inline constexpr float kVelocityDamp = 1.2f;

	// シミュレーションに使う最大dt（大きすぎると発散するので上限を設ける）
	inline constexpr float kMaxDeltaTime = 1.0f / 30.0f;

	// 注水：着水点の水面が上がる速さ（メートル/秒）
	inline constexpr float kPourRisePerSec = 2.5f;

	// 注水の広がり（着水点から左右に何列ぶんに水を足すか）
	inline constexpr int kPourHalfWidth = 3;

	// 開始時に入れておく水位（0.0～1.0）：描画確認しやすいよう少し入れておく
	inline constexpr float kInitialFillRate = 0.3f;

	// 水の色（上面・底で濃淡をつける）
	inline const Math::Color kColorTop    = { 0.35f, 0.70f, 0.98f, 1.0f };
	inline const Math::Color kColorBottom = { 0.13f, 0.42f, 0.82f, 1.0f };

	// 自己発光の強さ（陰影で暗くなりすぎず水色が見えるように）
	inline constexpr float kEmissiveRate = 0.5f;
}
