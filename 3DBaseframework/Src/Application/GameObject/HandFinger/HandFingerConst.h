#pragma once

// ===================================================
// 指でボタンを押す手（HandFinger）の定数
//  3Dの板ポリゴン（DrawPolygon）に手の画像を貼り、ワールド空間へ置く。
//  注水中（SPACE押下中）は指が押し込まれる方向へ動く。
//  ※位置・大きさは現場合わせなので ImGui(Tuning) で調整できるよう mutable にしている。
// ===================================================
namespace HandFingerConst
{
	inline constexpr const char* kImage = "Asset/Textures/HandFinger.png";

	// 板ポリの配置（ワールド座標。カメラは-Z側から+Zを見る＝手前ほどZが小さい）
	inline float kPosX = 1.5f;
	inline float kPosY = -1.0f;
	inline float kPosZ = -3.0f;

	// 板ポリの大きさ（ワールド単位。元画像は縦長なので 1:2 くらいが自然）
	inline float kWidth  = 2.0f;
	inline float kHeight = 4.0f;

	// 押したときに動く量（ワールド単位）。+Yで上（＝指がボタンを押し込む向き）。
	inline float kPressOffsetY = 0.18f;

	// 押す/離すの追従の滑らかさ（0〜1。大きいほどキビキビ）
	inline constexpr float kPressLerp = 0.35f;
}
