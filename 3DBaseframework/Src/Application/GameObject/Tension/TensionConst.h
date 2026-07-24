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

	// ゲームシーンのBGM（ループ）。心臓音と同じく、普通と籠り(muffled)を同時に鳴らして
	// 水が溜まるほど「普通→籠り」へ音量クロスフェードする。
	inline constexpr const char* kBgmAudio        = "Asset/Data/Audio/title.wav";
	inline constexpr const char* kBgmMuffledAudio = "Asset/Data/Audio/title_muffled.wav";

	// BGM全体の音量（クロスフェードしても合計はこの値を保つ）
	inline constexpr float kBgmVolume = 0.55f;

	// 籠りBGMが無い（ファイル未配置）ときに、代わりにどこまで音量を絞るか。
	//  ※クロスフェード相手が居なくても「水が溜まると引っ込む」感じは出しておく。
	inline constexpr float kBgmVolumeNoMuffled = 0.20f;

	// レターボックス（上下の黒帯）用の黒画像
	inline constexpr const char* kBlackImage = "Asset/Textures/kuro.png";

	// 黒帯（レターボックス）の高さ(px)。開始(intensity=0)は薄く、目標に近づく(1)ほど閉じて太くなる。
	inline constexpr float kMinBarHeight = 24.0f;	// 開始時＝薄い帯
	inline constexpr float kMaxBarHeight = 130.0f;	// 目標付近＝閉じた太い帯
	inline constexpr int   kBarWidth     = 1400;	// 帯の横幅（1280ウィンドウを覆う）

	// 画面の半分の高さ(px)。ウィンドウ720の半分＝上端+360／下端-360。
	inline constexpr float kScreenHalfHeight = 360.0f;
	// 帯を画面外へはみ出させる量(px)。整数丸めで端に1pxの隙間ができるのを防ぐ。
	inline constexpr float kBarOvershoot = 8.0f;

	// カメラのズーム量（ワールド単位）。intensityで最大この距離ぶんコップへ寄る。
	inline constexpr float kZoomAmount = 3.0f;

	// 手持ちカメラ風の揺れ。複数の正弦波を重ねて不規則に見せる（ワールド単位）。
	inline constexpr float kSwayAmpX  = 0.055f;	// 横揺れの大きさ
	inline constexpr float kSwayAmpY  = 0.040f;	// 縦揺れの大きさ
	inline constexpr float kSwaySpeed = 0.025f;	// 揺れの速さ（1フレームあたりの位相。小さいほどゆっくり）
	// 緊張が高いほど揺れを増やす倍率（intensity=1でこの倍）。1.0で一定。
	inline constexpr float kSwayIntensityGain = 1.8f;

	// 心臓音の音量：開始（空）は小さく、fillが溜まるほど大きくする。
	inline constexpr float kHeartVolMin = 0.25f;	// 開始時（intensity=0）の音量（小さい）
	inline constexpr float kHeartVolMax = 1.0f;		// 目標付近（intensity=1）の音量（大きい）

	// 音量・ズーム・帯の追従の滑らかさ（0〜1。小さいほどゆっくり変化＝「少しずつ」）
	inline constexpr float kLerpRate = 0.04f;
}
