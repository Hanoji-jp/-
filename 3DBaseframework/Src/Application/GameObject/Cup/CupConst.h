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

	// 目標ライン（内側領域を 0.0=底 ～ 1.0=上端 とした割合）
	const float kTargetLineRate = 0.7f;
	const float kTargetLineY    = kInnerBottomY + kInnerHeight * kTargetLineRate;

	// コップ枠・目標ラインの色
	const Math::Color kWallColor = { 0.85f, 0.90f, 0.95f, 1.0f };
	const Math::Color kLineColor = { 1.00f, 0.25f, 0.25f, 1.0f };

	// このコップを見るカメラのZ距離
	const float kCameraDistanceZ = 8.0f;
}
