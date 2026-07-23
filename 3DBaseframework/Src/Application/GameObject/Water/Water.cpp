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

	// コップ内側の矩形に、可視化テクスチャを貼る板ポリゴンを用意する
	//  UV: テクスチャ row0(上) をコップ上端へ対応させる（V=0が上）
	const float leftX  = CupConst::kInnerLeftX;
	const float rightX = CupConst::kInnerRightX;
	const float botY   = CupConst::kInnerBottomY;
	const float topY   = CupConst::kInnerTopY;

	std::vector<KdPolygon::Vertex> verts(4);
	verts[0].pos = { leftX,  botY, 0.0f };	verts[0].UV = { 0.0f, 1.0f };	// 左下
	verts[1].pos = { leftX,  topY, 0.0f };	verts[1].UV = { 0.0f, 0.0f };	// 左上
	verts[2].pos = { rightX, botY, 0.0f };	verts[2].UV = { 1.0f, 1.0f };	// 右下
	verts[3].pos = { rightX, topY, 0.0f };	verts[3].UV = { 1.0f, 0.0f };	// 右上
	m_displayPoly.SetVertices(verts);

	// 可視化テクスチャをベースカラーとして貼る（毎フレーム中身が更新される）
	m_displayPoly.SetMaterial(m_fluid->GetDisplayTexture());

	// 空のコップから開始。一発勝負の状態は未注水
	m_pourState = PourState::Ready;
	m_result    = UIConst::RyoResult::None;

	m_drawType = eDrawTypeUnLit;
}

void Water::Reset()
{
	if (m_fluid) { m_fluid->Reset(); }
	m_pourState = PourState::Ready;
	m_result    = UIConst::RyoResult::None;
}

void Water::SetPouring(bool pouring)
{
	// 一発勝負：一度終了したら以後は受け付けない
	if (m_pourState == PourState::Done) { return; }

	if (pouring)
	{
		m_pourState = PourState::Pouring;
	}
	else if (m_pourState == PourState::Pouring)
	{
		// 注いでいた手を離した → 終了して判定を確定
		m_pourState = PourState::Done;
		JudgeResult();
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
	if (!m_fluid) { return; }

	// GPUシミュレーションを1ステップ進める（レンダーターゲット切替を含む）
	float deltaTime = Application::Instance().GetDeltaTime();
	if (deltaTime > WaterConst::kMaxDeltaTime) { deltaTime = WaterConst::kMaxDeltaTime; }

	// 注水中フラグは「Pouring 状態のときだけ」true
	m_fluid->Step(deltaTime, m_pourState == PourState::Pouring);
}

void Water::Update()
{
	// 更新はGPU側（PreDraw）で行うため、ここでは何もしない
}

void Water::DrawUnLit()
{
	if (!m_fluid) { return; }

	// コップ内側へ、流体の可視化テクスチャを板ポリゴンで表示する
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_displayPoly, Math::Matrix::Identity, kWhiteColor);
}
