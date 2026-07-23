#pragma once

// ===================================================
// ImGuiでの実行時調整＋CSV保存/読込
//  コップのスケール／水の出る位置（ノズル）／ドリンクバーの位置・スケールを
//  実機でスライダー調整し、CSVに保存できる。起動時にCSVを読み込む。
// ===================================================
namespace Tuning
{
	// ImGui調整ウィンドウを描画（KdDebugGUI::GuiProcess から呼ぶ）
	void DrawImGui();

	// 現在の値を tuning.csv に保存
	void SaveCsv();

	// tuning.csv から読み込んで各値へ反映（起動時に呼ぶ）
	void LoadCsv();
}
