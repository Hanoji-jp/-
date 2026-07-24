#pragma once

// ===================================================
// 緊張演出（心臓音＋ズーム＋レターボックス）の定数
//  水位が目標ラインに近づくほど intensity(0→1) が上がり、
//   ・心臓音を「普通→籠り(muffled)」へクロスフェード
//   ・コップへ少しずつズームイン
//   ・上下の黒帯(レターボックス)を少しずつ縮める
//  ※intensity は fillRate / 目標ライン(CupConst::kTargetLineRate) で算出。
// ===================================================
namespace TensionConst
{
	// 心臓音（一回の鼓動をループ）。普通と籠りを鳴らし、音量で切り替える。
	inline constexpr const char* kHeartAudio   = "Asset/Data/Audio/heart.wav";
	inline constexpr const char* kMuffledAudio = "Asset/Data/Audio/heart_muffled.wav";

	// レターボックス（上下の黒帯）用の黒画像
	inline constexpr const char* kBlackImage = "Asset/Textures/kuro.png";

	// intensity=0（目標から遠い）のときの黒帯の高さ(px)。intensityが上がるほど縮む→0。
	inline constexpr float kMaxBarHeight = 110.0f;
	inline constexpr int   kBarWidth     = 1400;	// 帯の横幅（1280ウィンドウを覆う）

	// カメラのズーム量（ワールド単位）。intensityで最大この距離ぶんコップへ寄る。
	inline constexpr float kZoomAmount = 3.0f;

	// 心臓音の音量：開始（空）は小さく、fillが溜まるほど大きくする。
	inline constexpr float kHeartVolMin = 0.25f;	// 開始時（intensity=0）の音量（小さい）
	inline constexpr float kHeartVolMax = 1.0f;		// 目標付近（intensity=1）の音量（大きい）

	// 音量・ズーム・帯の追従の滑らかさ（0〜1。小さいほどゆっくり変化＝「少しずつ」）
	inline constexpr float kLerpRate = 0.04f;
}
