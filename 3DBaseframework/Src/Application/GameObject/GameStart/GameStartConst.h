#pragma once

// ===================================================
// ゲーム開始演出（イントロ）の定数
//  「水をドンピシャで入れろ！！！」→「3・2・1」→「始めっ！！！」を
//  音声＋画像で順番に出してから、注水を解禁する。
//  ※画像/音声はここのパスを差し替えるだけで変更できる（空文字なら出さない/鳴らさない）。
//   足りないアセットは後から Asset に置いて、このパスを指すだけでOK。
// ===================================================
namespace GameStartConst
{
	// 各フェーズの長さ（フレーム数。60fps基準）
	inline constexpr int kInstrFrames = 130;	// 「水をドンピシャで入れろ」表示（約2.2秒）
	inline constexpr int kCountFrames = 55;		// 数字ひとつぶん（約0.9秒）
	inline constexpr int kGoFrames    = 70;		// 「始めっ」表示（約1.2秒）

	// -------- 画像パス（空文字＝表示しない。数字画像が無いのでカウントは音声のみ） --------
	//  ※kInstrImage は「パチンコ大当たり風」の専用画像を用意したら、ここを差し替える（今は仮画像）。
	inline constexpr const char* kInstrImage = "Asset/Textures/title_kari.png";	// 仮：パチンコ大当たり画像に差し替え
	inline constexpr const char* kGoImage    = "Asset/Textures/space.png";		// 仮：「始めっ」専用画像に差し替え

	// 全画面を少し暗くする暗幕（イントロ中、ゲーム画面の上に薄く重ねて文字を見やすく）
	inline constexpr const char* kDarkImage  = "Asset/Textures/kuro.png";
	inline constexpr float       kDarkAlpha  = 0.35f;

	// -------- 音声パス（空文字＝鳴らさない。存在しないファイルを鳴らすと落ちるので注意） --------
	inline constexpr const char* kInstrAudio  = "Asset/Data/Audio/start.mp3";	// 「水をドンピシャで入れろ」
	inline constexpr const char* kCount3Audio = "Asset/Data/Audio/3.mp3";
	inline constexpr const char* kCount2Audio = "Asset/Data/Audio/2.mp3";
	inline constexpr const char* kCount1Audio = "Asset/Data/Audio/1.mp3";
	inline constexpr const char* kGoAudio     = "";								// 「始めっ」の音声（あれば "Asset/Data/Audio/xxx.mp3"）

	// スプライト表示サイズ（中心原点・ピクセル）。画像に合わせて調整。
	inline constexpr int kInstrWidth  = 1000;
	inline constexpr int kInstrHeight = 300;
	inline constexpr int kGoWidth     = 700;
	inline constexpr int kGoHeight    = 300;
	inline constexpr int kOverlayWidth  = 2400;	// 暗幕（全画面を覆う十分な大きさ）
	inline constexpr int kOverlayHeight = 1600;
}
