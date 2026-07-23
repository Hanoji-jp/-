#pragma once

// ===================================================
// ピッチャー（水を注ぐ器）関連の定数
//  濃い茶色の不透明な回転体。コップの上あたりに傾けて置き、ここから水を注ぐ。
// ===================================================
namespace PitcherConst
{
	// 濃い茶色（不透明）
	inline const Math::Color kColor = { 0.34f, 0.19f, 0.10f, 1.0f };

	// 回転体プロファイル（半径 r=x, 高さ y）。下→上の順。ジャグ（水差し）の輪郭。
	inline const std::vector<Math::Vector2> kProfile =
	{
		{ 0.70f, 0.0f },	// 底
		{ 1.00f, 0.3f },
		{ 1.15f, 1.2f },	// 胴の膨らみ
		{ 0.85f, 2.2f },
		{ 0.75f, 2.9f },	// 首
		{ 0.95f, 3.3f },	// 口（少し開く）
	};

	inline constexpr int kSegments = 28;	// 回転分割
	inline constexpr int kRings    = 20;	// 高さ分割

	// 配置（机の上・コップ中央の真上に口が来るよう傾けて置く）※スクショで調整する値
	inline constexpr float kScale   = 0.4f;
	inline constexpr float kTiltDeg = -115.0f;	// 口を下向きに（注ぐ姿勢）。コップの縁より上・外に置く。
	// 口とコップは注ぎストリームで繋ぐので、ピッチャーは自然な位置（コップの奥行きに合わせる）。
	inline const Math::Vector3 kPos = { -1.0f, 1.45f, -2.0f };

	inline Math::Matrix WorldMatrix()
	{
		const float rad = kTiltDeg * 3.14159265f / 180.0f;
		return Math::Matrix::CreateScale(kScale) *
			   Math::Matrix::CreateRotationZ(rad) *
			   Math::Matrix::CreateTranslation(kPos);
	}

	// 注ぎ口（ローカル座標）＝開口のふち（+X側＝傾けたとき一番下になる注ぎ縁）。
	//  開口の中心だとピッチャー内側から水柱が出て被るので、外縁を始点にする。※スクショで調整。
	inline const Math::Vector3 kSpoutLocal = { 0.95f, 3.3f, 0.0f };

	// 注ぎ口のワールド座標
	inline Math::Vector3 SpoutWorldPos()
	{
		return Math::Vector3::Transform(kSpoutLocal, WorldMatrix());
	}
}
