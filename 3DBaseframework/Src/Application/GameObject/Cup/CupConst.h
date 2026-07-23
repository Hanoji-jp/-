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
}
