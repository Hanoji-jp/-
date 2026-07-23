#include "Water.h"
#include "WaterConst.h"
#include "FluidField.h"
#include "../Cup/CupConst.h"
#include "../../main.h"

// ===================================================
// 水本体
// ===================================================

// コンストラクタ／デストラクタは、前方宣言の FluidField を
// 完全型として扱える .cpp 側で定義する（unique_ptr の破棄に必要）
Water::Water() {}
Water::~Water() {}

void Water::Init()
{
	// 2次元流体場（GPU）を生成
	m_fluid = std::make_unique<FluidField>();
	m_fluid->Init();

	// 流体シム領域（コップ本体＋上のシュート）全体に、可視化テクスチャを貼る板ポリゴン（2.5D表示）。
	//  縦は kSimTopY（シュート頂上＝水源）～ kSimBottomY（コップ底）まで。落水路も含めて1枚で表示する。
	//  UV: テクスチャ row0(上=シュート頂上) を上端へ対応させる（V=0が上）。
	const float leftX  = CupConst::kInnerLeftX;
	const float rightX = CupConst::kInnerRightX;
	const float botY   = CupConst::kSimBottomY;
	const float topY   = CupConst::kSimTopY;

	std::vector<KdPolygon::Vertex> verts(4);
	verts[0].pos = { leftX,  botY, 0.0f };	verts[0].UV = { 0.0f, 1.0f };	// 左下
	verts[1].pos = { leftX,  topY, 0.0f };	verts[1].UV = { 0.0f, 0.0f };	// 左上
	verts[2].pos = { rightX, botY, 0.0f };	verts[2].UV = { 1.0f, 1.0f };	// 右下
	verts[3].pos = { rightX, topY, 0.0f };	verts[3].UV = { 1.0f, 0.0f };	// 右上
	m_displayPoly.SetVertices(verts);

	// 可視化テクスチャをベースカラーとして貼る（毎フレーム流体シムで更新される）
	m_displayPoly.SetMaterial(m_fluid->GetDisplayTexture());

	// 空のコップから開始。一発勝負の状態は未注水
	m_pourState = PourState::Ready;
	m_result    = UIConst::RyoResult::None;

	m_drawType = eDrawTypeUnLit;
}

void Water::Reset()
{
	if (m_fluid) { m_fluid->Reset(); }
	m_pourState   = PourState::Ready;
	m_settleFrames = 0;
	m_result      = UIConst::RyoResult::None;
}

void Water::SetPouring(bool pouring)
{
	// 一発勝負：一度終了したら以後は受け付けない
	if (m_pourState == PourState::Done) { return; }

	if (pouring)
	{
		// 沈静中に押し直したら注水を再開（まだ確定していないので許可）
		m_pourState = PourState::Pouring;
	}
	else if (m_pourState == PourState::Pouring)
	{
		// 注いでいた手を離した → すぐ判定せず「沈静」へ。水が落ち着いてから判定する。
		m_pourState  = PourState::Settling;
		m_settleFrames = 0;
	}
}

void Water::JudgeResult()
{
	// この時点の水位（＝泡が全部落ち着いた後の最終液面）で判定
	const float rate   = GetFillRate();
	const float target = CupConst::kTargetLineRate;
	const float tol    = UIConst::kRyoTolerance;

	if (rate < target - tol)      { m_result = UIConst::RyoResult::Under; }
	else if (rate > target + tol) { m_result = UIConst::RyoResult::Over; }
	else                          { m_result = UIConst::RyoResult::Ryo; }
}

float Water::GetFillRate() const
{
	return m_fluid ? m_fluid->GetFillRate() : 0.0f;
}

void Water::PreDraw()
{
	// ImGuiで粒の細かさ（グリッド解像度）を変えたら、流体場を作り直して反映する。
	//  テクスチャ類は解像度依存なので、Release→Init で全リソースを新解像度で再生成する。
	if (WaterConst::g_fluidReinitRequested)
	{
		WaterConst::g_fluidReinitRequested = false;
		m_fluid = std::make_unique<FluidField>();	// 旧インスタンスは破棄され解放される
		m_fluid->Init();
		m_displayPoly.SetMaterial(m_fluid->GetDisplayTexture());	// 新しい表示テクスチャを貼り直す
		m_pourState   = PourState::Ready;
		m_settleFrames = 0;
		m_result      = UIConst::RyoResult::None;
	}

	if (!m_fluid) { return; }

	// 判定確定後（Done）は流体を止めて水面を完全に固定する（＝落ち着いた水はもう動かさない）。
	//  グリッド流体は回し続けると数値拡散で必ず水位がわずかに動く。反拡散で打ち消そうとすると
	//  逆に過剰集中で暴走する（検証済み）。静止した水を計算し続ける必要はないので、確定後は
	//  ステップを止めて表示テクスチャ・水位をそのまま保持する（物理エンジンの sleep と同じ）。
	if (m_pourState == PourState::Done)
	{
		CupConst::UpdateTilt(false);
		return;
	}

	// GPUシミュレーションを1ステップ進める（レンダーターゲット切替を含む）
	float deltaTime = Application::Instance().GetDeltaTime();
	if (deltaTime > WaterConst::kMaxDeltaTime) { deltaTime = WaterConst::kMaxDeltaTime; }

	// 注水中フラグは「Pouring 状態のときだけ」true
	const bool pouring = (m_pourState == PourState::Pouring);
	m_fluid->Step(deltaTime, pouring);

	// デバッグ表示用に実測水位を毎フレーム公開（ImGuiで見た目とのズレを確認・調整）
	WaterConst::g_debugFillRate = m_fluid->GetFillRate();

	// 手を離した後は「沈静」中：水が落ち着くまで待ってから判定を確定する。
	//  暴れている最中に判定すると水面が読めず、良（クリア）が正しく出ないため。
	if (m_pourState == PourState::Settling)
	{
		if (++m_settleFrames >= WaterConst::kJudgeSettleFrames)
		{
			JudgeResult();				// 落ち着いた水面で判定
			m_pourState = PourState::Done;
		}
	}

	// 注水中はコップを少し傾ける（水位を読みにくくして難度up）。手を離すと真っ直ぐへ戻る。
	CupConst::UpdateTilt(pouring);
}

void Water::Update()
{
	// 更新はGPU側（PreDraw）で行うため、ここでは何もしない
}

// 水は流体シムの可視化テクスチャを貼った2.5D平面。動的な波・流れをそのまま見せる。
//  不透明シーン（机）の後・ガラスの前に描く DrawEffect で半透明合成する。
//  オブジェクト順で Water は Cup より前なので、水→ガラスの順に描かれ、ガラス越しに水が見える。
void Water::DrawEffect()
{
	if (!m_fluid) { return; }

	// コップのガラス・目標ラインと同じ配置行列（机の上）で表示する。
	//  低解像度の可視化テクスチャを線形補間で拡大し、ふちを滑らかにする。
	//  深度テスト（ZWriteDisable）：手前の不透明物（ピッチャー等）に隠れる＝水が突き抜けない。
	//   自身は深度を書かないので、後で描く半透明ガラスと正しく重なる。
	auto& sm = KdShaderManager::Instance();
	sm.ChangeSamplerState(KdSamplerState::Linear_Clamp);
	sm.ChangeBlendState(KdBlendState::Alpha);
	sm.ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
	sm.m_StandardShader.DrawPolygon(m_displayPoly, CupConst::PlacementMatrix(), kWhiteColor);
	sm.UndoDepthStencilState();
	sm.UndoBlendState();
	sm.UndoSamplerState();
}
