#include "../../main.h"
#include "Tuning.h"
#include "../Cup/CupConst.h"
#include "../DrinkBar/DrinkBarConst.h"
#include "../Water/WaterConst.h"	// 実測水位のデバッグ表示
#include "../UI/UIConst.h"			// 判定の許容誤差
#include "../HandFinger/HandFingerConst.h"	// 指の位置・サイズ調整
#include <fstream>
#include <string>
#include <cstdlib>

namespace
{
	// 作業ディレクトリ（3DBaseframework）直下に保存する
	const char* kCsvPath = "tuning.csv";

	// 調整ウィンドウは普段は非表示（プレイ画面を隠さないため）。このキーで表示／非表示を切り替える。
	constexpr int  kToggleKey      = VK_F1;
	constexpr bool kDefaultVisible = false;

	// 現在の表示状態と、切替キーの前フレーム押下状態（押した瞬間だけ反転させる）
	bool g_visible       = kDefaultVisible;
	bool g_toggleKeyPrev = false;
}

void Tuning::DrawImGui()
{
	// F1で表示／非表示を切り替える（初期は非表示）
	const bool toggleDown = (GetAsyncKeyState(kToggleKey) & 0x8000) != 0;
	if (toggleDown && !g_toggleKeyPrev) { g_visible = !g_visible; }
	g_toggleKeyPrev = toggleDown;

	if (!g_visible) { return; }

	// 内容に合わせて自動リサイズ（追加した項目が下に隠れないように）
	if (ImGui::Begin("Tuning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// 判定デバッグ：実測水位・目標ライン・良の帯を表示（見た目とのズレ確認用）
		const float fill   = WaterConst::g_debugFillRate;
		const float target = CupConst::kTargetLineRate;
		const float tol    = UIConst::kRyoTolerance;
		const bool  inRyo  = (fill >= target - tol && fill <= target + tol);
		ImGui::Text("fillRate: %.3f", fill);
		ImGui::SameLine();
		ImGui::TextColored(inRyo ? ImVec4(0.3f, 1.0f, 0.3f, 1.0f) : ImVec4(1.0f, 0.6f, 0.3f, 1.0f),
			inRyo ? "[RYO]" : "[--]");
		ImGui::Text("target %.2f  tol +/-%.2f  (band %.2f..%.2f)", target, tol, target - tol, target + tol);
		ImGui::Separator();

		ImGui::Text("Cup (Glass + Water)");
		ImGui::SliderFloat("Cup Scale", &CupConst::kPlacementScale, 0.1f, 2.0f);
		ImGui::SliderFloat("Cup Y",     &CupConst::kPlacementY,    -5.0f, 5.0f);
		ImGui::SliderFloat("Cup Z",     &CupConst::kPlacementZ,    -6.0f, 4.0f);

		ImGui::Separator();
		ImGui::Text("DrinkBar");
		ImGui::SliderFloat("Bar Scale", &DrinkBarConst::kScale, 0.2f, 8.0f);
		ImGui::SliderFloat3("Bar Pos (XYZ)", &DrinkBarConst::kPos.x, -8.0f, 8.0f);

		ImGui::Separator();
		ImGui::Text("Water outlet (nozzle, local)");
		ImGui::SliderFloat3("Nozzle (XYZ)", &DrinkBarConst::kNozzleLocal.x, -3.0f, 3.0f);

		ImGui::Separator();
		ImGui::Text("Pour / Fall speed");
		ImGui::SliderFloat("Fall Speed", &WaterConst::kGravityPerStep, 0.03f, 0.30f);	// 落ちる速さ（重力）。小さいほどゆっくり注ぐ
		ImGui::SliderFloat("Pour Rate", &WaterConst::kPourMassPerStep, 0.05f, 1.5f);	// 1ステップの水量（太さ・満ちる速さ）
		ImGui::SliderFloat("Pour Velocity", &WaterConst::kPourVelocity, 0.0f, 1.0f);	// 注ぎ口での初速
		// 反拡散：止水後に水面が下がらないよう拡散を打ち消す強さ。上げ過ぎると振動/発散。
		ImGui::SliderFloat("Anti-Diffuse", &WaterConst::kAntiDiffusionRate, 0.0f, 0.20f);

		ImGui::Separator();
		ImGui::Text("Hand (finger button, 3D)");
		ImGui::SliderFloat("Hand X", &HandFingerConst::kPosX, -8.0f, 8.0f);
		ImGui::SliderFloat("Hand Y", &HandFingerConst::kPosY, -8.0f, 8.0f);
		ImGui::SliderFloat("Hand Z", &HandFingerConst::kPosZ, -8.0f, 8.0f);
		ImGui::SliderFloat("Hand W", &HandFingerConst::kWidth, 0.1f, 10.0f);
		ImGui::SliderFloat("Hand H", &HandFingerConst::kHeight, 0.1f, 10.0f);
		ImGui::SliderFloat("Hand Press", &HandFingerConst::kPressOffsetY, 0.0f, 1.0f);

		ImGui::Separator();
		ImGui::Text("Water particles (grid res)  now %dx%d", WaterConst::kGridWidth, WaterConst::kGridHeight);
		// 粒の細かさ＝基準(128x320)に対する倍率%。上げるほど粒が小さく（細かく）なる。上げ過ぎると重い。
		static int fineness = 125;	// 既定 160x400 = 125%
		ImGui::SliderInt("Fineness %", &fineness, 100, 175);	// 上げ過ぎ（重すぎ）防止で上限175%
		if (ImGui::Button("Apply particles (smaller=finer)"))
		{
			WaterConst::kGridWidth  = ((128 * fineness / 100) / 2) * 2;	// 偶数に丸める
			WaterConst::kGridHeight = ((320 * fineness / 100) / 2) * 2;
			WaterConst::g_fluidReinitRequested = true;	// 流体を作り直して反映
		}
		ImGui::TextDisabled("(Apply = rebuild fluid. higher = heavier)");

		ImGui::Separator();
		if (ImGui::Button("Save CSV")) { SaveCsv(); }
		ImGui::SameLine();
		if (ImGui::Button("Load CSV")) { LoadCsv(); }
		ImGui::SameLine();
		ImGui::TextDisabled("(%s)", kCsvPath);
	}
	ImGui::End();
}

void Tuning::SaveCsv()
{
	std::ofstream ofs(kCsvPath, std::ios::trunc);
	if (!ofs) { return; }

	ofs << "cupScale," << CupConst::kPlacementScale << "\n";
	ofs << "cupY,"     << CupConst::kPlacementY     << "\n";
	ofs << "cupZ,"     << CupConst::kPlacementZ     << "\n";
	ofs << "barScale," << DrinkBarConst::kScale     << "\n";
	ofs << "barX,"     << DrinkBarConst::kPos.x      << "\n";
	ofs << "barY,"     << DrinkBarConst::kPos.y      << "\n";
	ofs << "barZ,"     << DrinkBarConst::kPos.z      << "\n";
	ofs << "nozX,"     << DrinkBarConst::kNozzleLocal.x << "\n";
	ofs << "nozY,"     << DrinkBarConst::kNozzleLocal.y << "\n";
	ofs << "nozZ,"     << DrinkBarConst::kNozzleLocal.z << "\n";
	ofs << "pourRate," << WaterConst::kPourMassPerStep  << "\n";
	ofs << "pourVel,"  << WaterConst::kPourVelocity      << "\n";
	ofs << "fallSpeed,"<< WaterConst::kGravityPerStep    << "\n";
	ofs << "gridW,"    << WaterConst::kGridWidth          << "\n";
	ofs << "gridH,"    << WaterConst::kGridHeight         << "\n";
	ofs << "handX,"    << HandFingerConst::kPosX          << "\n";
	ofs << "handY,"    << HandFingerConst::kPosY          << "\n";
	ofs << "handZ,"    << HandFingerConst::kPosZ          << "\n";
	ofs << "handW,"    << HandFingerConst::kWidth         << "\n";
	ofs << "handH,"    << HandFingerConst::kHeight        << "\n";
	ofs << "handPress,"<< HandFingerConst::kPressOffsetY  << "\n";
}

void Tuning::LoadCsv()
{
	std::ifstream ifs(kCsvPath);
	if (!ifs) { return; }

	std::string line;
	while (std::getline(ifs, line))
	{
		const size_t comma = line.find(',');
		if (comma == std::string::npos) { continue; }

		const std::string key = line.substr(0, comma);
		const float v = static_cast<float>(std::atof(line.substr(comma + 1).c_str()));

		if      (key == "cupScale") { CupConst::kPlacementScale = v; }
		else if (key == "cupY")     { CupConst::kPlacementY     = v; }
		else if (key == "cupZ")     { CupConst::kPlacementZ     = v; }
		else if (key == "barScale") { DrinkBarConst::kScale     = v; }
		else if (key == "barX")     { DrinkBarConst::kPos.x      = v; }
		else if (key == "barY")     { DrinkBarConst::kPos.y      = v; }
		else if (key == "barZ")     { DrinkBarConst::kPos.z      = v; }
		else if (key == "nozX")     { DrinkBarConst::kNozzleLocal.x = v; }
		else if (key == "nozY")     { DrinkBarConst::kNozzleLocal.y = v; }
		else if (key == "nozZ")     { DrinkBarConst::kNozzleLocal.z = v; }
		else if (key == "pourRate") { WaterConst::kPourMassPerStep = v; }
		else if (key == "pourVel")  { WaterConst::kPourVelocity     = v; }
		else if (key == "fallSpeed"){ WaterConst::kGravityPerStep   = v; }
		else if (key == "gridW")    { WaterConst::kGridWidth  = static_cast<int>(v); }
		else if (key == "gridH")    { WaterConst::kGridHeight = static_cast<int>(v); }
		else if (key == "handX")    { HandFingerConst::kPosX         = v; }
		else if (key == "handY")    { HandFingerConst::kPosY         = v; }
		else if (key == "handZ")    { HandFingerConst::kPosZ         = v; }
		else if (key == "handW")    { HandFingerConst::kWidth        = v; }
		else if (key == "handH")    { HandFingerConst::kHeight       = v; }
		else if (key == "handPress"){ HandFingerConst::kPressOffsetY = v; }
	}
}
