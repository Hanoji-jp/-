#pragma once

// ===================================================
// ドリンクバー（ディスペンサー）関連の定数
//  ノズルは下向き固定。コップをノズルの真下に置いて水を注ぐ。
//  配置・ノズル位置はモデルに合わせてスクショで調整する。
// ===================================================
namespace DrinkBarConst
{
	// ※ImGuiで実行時に調整＋CSV保存するため可変(inline)にしている
	inline float kScale = 3.6f;						// 大きさ
	inline Math::Vector3 kPos = { 0.0f, -2.15f, 2.2f };	// 位置（底が机の天面に着くよう調整）

	inline Math::Matrix WorldMatrix()
	{
		return Math::Matrix::CreateScale(kScale) *
			   Math::Matrix::CreateTranslation(kPos);
	}

	// ノズル（水が出る口）のローカル座標。モデルに合わせて調整＝注ぎストリームの始点。（可変）
	inline Math::Vector3 kNozzleLocal = { 0.0f, 0.4f, -0.2f };

	// ノズルのワールド座標
	inline Math::Vector3 NozzleWorldPos()
	{
		return Math::Vector3::Transform(kNozzleLocal, WorldMatrix());
	}
}
