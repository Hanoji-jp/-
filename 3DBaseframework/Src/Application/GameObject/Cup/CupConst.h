#pragma once

// ===================================================
// コップ関連の定数
//  3Dワールド空間（単位=メートル）。原点を画面中央とし、右+X・上+Y・手前+Z
// ===================================================
namespace CupConst
{
	// コップ内側（水が溜まる領域）の横幅・高さ
	const float kInnerWidth  = 4.0f;
	const float kInnerHeight = 5.0f;

	// 内側領域の中心X・底のY
	const float kCenterX      = 0.0f;
	const float kInnerBottomY = -2.5f;
	const float kInnerTopY    = kInnerBottomY + kInnerHeight;
	const float kInnerLeftX   = kCenterX - kInnerWidth * 0.5f;
	const float kInnerRightX  = kCenterX + kInnerWidth * 0.5f;

	// ===================================================
	// 流体シミュレーション領域（コップ内側の「上」に落水路シュートを足して拡張）
	//  ノズル→コップの落下も含めて1つの流体として解く。別物の水柱は作らない。
	//  表示クアッド・形状マスク・水位測定でこの領域を共有する。
	//   ・縁より下＝コップ本体（多角形 kInnerShape）
	//   ・縁より上＝細い中央チャネル（落水を一定の太さに保ち、縁で急に細くならない）
	// ===================================================
	const float kChuteHeight    = 5.0f;						// 縁より上のシュート高さ（局所単位）
	const float kSimHeight      = kInnerHeight + kChuteHeight;	// 流体領域の全高
	const float kSimTopY        = kInnerTopY + kChuteHeight;	// 領域上端Y＝水源＝ノズル高さ
	const float kSimBottomY     = kInnerBottomY;				// 領域下端Y（コップ底）
	const float kChuteHalfWidth = 0.7f;						// シュート中央チャネルの半幅（落水の太さ）
	const float kSimRimFrac     = kChuteHeight / kSimHeight;	// 縁の行割合（0=上端,1=下端。これ未満＝シュート）

	// コップ内側の輪郭（自由多角形。世界座標。上辺＝開口＝水面）
	//  水はこの多角形の内側にだけ溜まる。頂点を差し替えればどんな形にもできる。
	//  並びは 上左(開口) → 左壁 → 底 → 右壁 → 上右(開口) の順。
	//  最後(上右)から最初(上左)へ戻る辺が「開口」＝壁は描かない。
	inline const std::vector<Math::Vector2> kInnerShape =
	{
		{ -2.0f,  2.5f },	// 上左（開口）
		{ -1.9f,  0.2f },
		{ -1.3f, -2.5f },	// 底左
		{  1.3f, -2.5f },	// 底右
		{  1.9f,  0.2f },
		{  2.0f,  2.5f },	// 上右（開口）
	};

	// 目標ライン（内側領域を 0.0=底 ～ 1.0=上端 とした割合）
	const float kTargetLineRate = 0.7f;
	const float kTargetLineY    = kInnerBottomY + kInnerHeight * kTargetLineRate;

	// コップ枠・目標ラインの色
	const Math::Color kWallColor = { 0.85f, 0.90f, 0.95f, 1.0f };
	const Math::Color kLineColor = { 1.00f, 0.25f, 0.25f, 1.0f };

	// このコップを見るカメラのZ距離
	const float kCameraDistanceZ = 8.0f;

	// ===================================================
	// 机の上への配置（コップのガラス・目標ライン・水の表示クアッドで共有する行列）
	//  ローカル座標は原点中心・底Y=-2.5・上端Y=2.5。これを机の天面へ乗せる。
	//  ※スクショで見ながら調整する値。
	// ===================================================
	// ※ImGuiで実行時に調整＋CSV保存するため inline 変数（可変）にしている
	inline float kPlacementScale = 0.65f;	// 大きさ（画面占有を抑えて小さめに）
	inline float kPlacementY     = -0.6f;	// 上下（縮小しても底が机の天面に乗るよう調整）
	inline float kPlacementZ     = -2.0f;	// 奥行き（机の位置へ寄せる）

	// 注水中はコップを少し傾けて、どれだけ注いだか読みにくくする（難度up）。
	//  手を離すと真っ直ぐに戻り、そこで最終水位が読める。コップ・水・目標ラインを一緒に傾ける。
	inline float g_cupTiltDeg  = 0.0f;		// 現在の傾き（毎フレーム更新）
	const float  kPourTiltDeg  = 0.0f;		// 注水中の目標傾き（度）。0＝傾けない（コップは真っ直ぐのまま）
	const float  kTiltLerpRate = 0.12f;		// 傾きの追従速度（0～1）

	// 注水状態に応じて傾きを滑らかに更新する（毎フレーム呼ぶ）
	inline void UpdateTilt(bool pouring)
	{
		const float target = pouring ? kPourTiltDeg : 0.0f;
		g_cupTiltDeg += (target - g_cupTiltDeg) * kTiltLerpRate;
	}

	inline Math::Matrix PlacementMatrix()
	{
		const float rad = g_cupTiltDeg * 3.14159265f / 180.0f;
		// 傾きの回転中心＝コップ上端（注ぎ口付近）。こうすると注水中に傾いても
		// 注ぎ口の位置がほぼ動かず、ピッチャーの口とつながったまま見える。
		const float pivotY = kInnerTopY * kPlacementScale;
		return Math::Matrix::CreateScale(kPlacementScale) *
			   Math::Matrix::CreateTranslation(0.0f, -pivotY, 0.0f) *
			   Math::Matrix::CreateRotationZ(rad) *
			   Math::Matrix::CreateTranslation(0.0f, pivotY, 0.0f) *
			   Math::Matrix::CreateTranslation(0.0f, kPlacementY, kPlacementZ);
	}

	// ===================================================
	// 3D半透明ガラス（回転体メッシュ）
	//  kInnerShape の右半分を半径 r(y) として Y軸回りに回転させて作る。
	// ===================================================
	const int   kGlassSegments  = 32;	// 回転方向の分割数（多いほど滑らか）
	const int   kGlassRings     = 16;	// 高さ方向の分割数（半径を補間して滑らかに）
	const float kGlassThickness = 0.08f;	// 内側形状より少し外へ（ガラスの肉厚ぶん）

	// ガラスの色（半透明）。α小さめで薄いガラスに。
	const Math::Color kGlassColor = { 0.80f, 0.90f, 1.00f, 0.22f };
}
