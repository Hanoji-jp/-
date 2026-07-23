#pragma once

// ===================================================
// ゲーム開始演出（イントロ）の定数
//  「3 → 2 → 1 → start（＝『水をドンピシャで入れろ！！！』）」の順で
//  音声＋画像を出してから、注水を解禁する。
//  ※画像/音声はここのパスを差し替えるだけで変更できる（空文字なら出さない/鳴らさない）。
// ===================================================
namespace GameStartConst
{
	// 各フェーズの長さ（フレーム数。60fps基準）
	inline constexpr int kCountFrames = 55;		// 数字ひとつぶん（約0.9秒）
	inline constexpr int kStartFrames = 90;		// 「水をドンピシャで入れろ」表示（約1.5秒）

	// -------- 画像パス（各フェーズで全画面表示） --------
	inline constexpr const char* kCount3Image = "Asset/Textures/3.png";
	inline constexpr const char* kCount2Image = "Asset/Textures/2.png";
	inline constexpr const char* kCount1Image = "Asset/Textures/1.png";
	inline constexpr const char* kStartImage  = "Asset/Textures/start.png";	// 水をドンピシャで入れろ

	// 全画面を少し暗くする暗幕（画像に透過がある場合に背後を暗くして見やすく）
	inline constexpr const char* kDarkImage  = "Asset/Textures/kuro.png";
	inline constexpr float       kDarkAlpha  = 0.35f;

	// -------- 音声パス（空文字＝鳴らさない。存在しないファイルを鳴らすと落ちるので注意） --------
	inline constexpr const char* kCount3Audio = "Asset/Data/Audio/3.mp3";
	inline constexpr const char* kCount2Audio = "Asset/Data/Audio/2.mp3";
	inline constexpr const char* kCount1Audio = "Asset/Data/Audio/1.mp3";
	inline constexpr const char* kStartAudio  = "Asset/Data/Audio/start.mp3";	// 「水をドンピシャで入れろ」

	// スプライト表示サイズ（中心原点・ピクセル）。ウィンドウは1280x720なので全画面で表示。
	inline constexpr int kImageWidth  = 1280;	// 全画面（WindowSettings.csv と合わせる）
	inline constexpr int kImageHeight = 720;
	inline constexpr int kOverlayWidth  = 2400;	// 暗幕（全画面を覆う十分な大きさ）
	inline constexpr int kOverlayHeight = 1600;
}
