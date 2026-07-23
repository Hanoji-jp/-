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

	// ===============================================
	// 2次元グリッド流体（GPUマルチパス）用の定数
	//  コップ内側の矩形領域に対応させたグリッドで水を解く。
	//  quantity テクセル = (x,y=運動量, z=質量, w=体積)
	// ===============================================

	// 流体グリッドの解像度（コップ内側 幅4.0×高さ5.0 の比 4:5 に合わせる）
	inline constexpr int kGridWidth  = 64;
	inline constexpr int kGridHeight = 80;

	// セル初期値：質量／体積
	inline constexpr float kCellWaterMass = 1.0f;	// 水で満たされたセルの質量
	inline constexpr float kCellAirMass   = 0.001f;	// 水の無いセルの質量（完全な0は避ける）
	inline constexpr float kCellVolume    = 1.0f;	// セルの体積（全セル共通）

	// 可視化パス（Phase1）の色：空色⇔水色を質量で補間して表示する
	inline const Math::Color kVisualizeSpaceColor = { 0.87f, 0.93f, 0.98f, 1.0f };
	inline const Math::Color kVisualizeWaterColor = { 0.20f, 0.55f, 0.90f, 1.0f };

	// ===============================================
	// Phase2：移流＋重力
	// ===============================================

	// 重力（1ステップあたりの落下量：グリッドセル単位。row増加＝画面下が +）
	inline constexpr float kGravityPerStep = 0.1f;

	// 初期の水ブロック（グリッドに対する割合。row0=上）
	//  上部中央に塊を置き、重力で落下して底に溜まる様子を確認する
	inline constexpr float kInitBlockLeftRate   = 0.25f;	// 左端
	inline constexpr float kInitBlockRightRate  = 0.75f;	// 右端
	inline constexpr float kInitBlockTopRate    = 0.08f;	// 上端（小さいほど上）
	inline constexpr float kInitBlockBottomRate = 0.42f;	// 下端

	// ===============================================
	// Phase3：圧力（マルチグリッド解法）
	// ===============================================

	// 各レベルでの圧力ヤコビ反復回数
	inline constexpr int kJacobiIterations = 5;

	// intensityピラミッドを作る最小サイズ（幅・高さがこれ未満になったら止める）
	inline constexpr int kMinPyramidSize = 8;

	// ===============================================
	// Phase4：注水（pour）と補填（equalization）
	// ===============================================

	// 吸い込み口（上部中央）の形（グリッドのセル単位）
	inline constexpr int kPourHalfCols = 3;	// 中心から左右へ何列ぶんか（幅 = 2*これ）
	inline constexpr int kPourTopRow   = 2;	// 上端から何row下に置くか
	inline constexpr int kPourRows     = 3;	// 縦方向の厚み（row数）

	// 1ステップで注ぐ量・下向き速度
	inline constexpr float kPourMassPerStep = 0.5f;	// セルあたりに加える質量
	inline constexpr float kPourVelocity    = 0.6f;	// 加える下向き速度

	// equalization（余剰を削り不足を補う率）
	inline constexpr float kEqualizationSurplusRate = 0.01f;
	inline constexpr float kEqualizationDeficitRate = 0.01f;
}
