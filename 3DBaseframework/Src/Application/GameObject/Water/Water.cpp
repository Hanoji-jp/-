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

	// 開始時の水位（Phase1では初期充填率を保持。注水はPhase4で連携）
	m_fillRate  = WaterConst::kInitialFillRate;
	m_isPouring = false;

	m_drawType = eDrawTypeUnLit;
}

void Water::Reset()
{
	if (m_fluid) { m_fluid->Reset(); }
	m_fillRate = WaterConst::kInitialFillRate;
}

void Water::PreDraw()
{
	if (!m_fluid) { return; }

	// GPUシミュレーションを1ステップ進める（レンダーターゲット切替を含む）
	float deltaTime = Application::Instance().GetDeltaTime();
	if (deltaTime > WaterConst::kMaxDeltaTime) { deltaTime = WaterConst::kMaxDeltaTime; }

	m_fluid->Step(deltaTime);
}

void Water::Update()
{
	// Phase1では水位は初期値を保持（注水・判定連携はPhase4で実装）
}

void Water::DrawUnLit()
{
	if (!m_fluid) { return; }

	// コップ内側へ、流体の可視化テクスチャを板ポリゴンで表示する
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_displayPoly, Math::Matrix::Identity, kWhiteColor);
}
