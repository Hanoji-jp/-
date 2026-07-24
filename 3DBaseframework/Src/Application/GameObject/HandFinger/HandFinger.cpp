#include "HandFinger.h"
#include "HandFingerConst.h"

void HandFinger::Init()
{
	// 手の画像を読み込み、板ポリのマテリアルに設定
	m_spTex = std::make_shared<KdTexture>();
	m_spTex->Load(HandFingerConst::kImage);

	// 中心原点の「1x1の単位クアッド」を作る。
	//  大きさは描画時のスケール行列で与えるので、ImGuiでサイズを変えても即反映される。
	//  頂点順は水の表示板と同じ（左下・左上・右下・右上＝トライアングルストリップ）。
	using Vertex = KdPolygon::Vertex;
	std::vector<Vertex> v(4);
	v[0].pos = { -0.5f, -0.5f, 0.0f };	v[0].UV = { 0.0f, 1.0f };	// 左下
	v[1].pos = { -0.5f,  0.5f, 0.0f };	v[1].UV = { 0.0f, 0.0f };	// 左上
	v[2].pos = {  0.5f, -0.5f, 0.0f };	v[2].UV = { 1.0f, 1.0f };	// 右下
	v[3].pos = {  0.5f,  0.5f, 0.0f };	v[3].UV = { 1.0f, 0.0f };	// 右上
	for (Vertex& vv : v) { vv.normal = { 0.0f, 0.0f, -1.0f }; }		// カメラ側を向く
	m_poly.SetVertices(v);
	m_poly.SetMaterial(m_spTex);

	m_pressed = false;
	m_press   = 0.0f;
}

void HandFinger::Update()
{
	// 押下状態へ滑らかに追従（カクつかずに押し込む/戻る）
	const float target = m_pressed ? 1.0f : 0.0f;
	m_press += (target - m_press) * HandFingerConst::kPressLerp;
}

void HandFinger::DrawEffect()
{
	// 押し込むほど上へ動かす（指がボタンを押している見え方）
	const float y = HandFingerConst::kPosY + HandFingerConst::kPressOffsetY * m_press;

	// 単位クアッドを 大きさ→位置 の順で配置
	const Math::Matrix world =
		Math::Matrix::CreateScale(HandFingerConst::kWidth, HandFingerConst::kHeight, 1.0f) *
		Math::Matrix::CreateTranslation(HandFingerConst::kPosX, y, HandFingerConst::kPosZ);

	// 画像の透過を活かして半透明合成。深度は書かず、手前の不透明物には隠れる。
	auto& sm = KdShaderManager::Instance();
	sm.ChangeSamplerState(KdSamplerState::Linear_Clamp);
	sm.ChangeBlendState(KdBlendState::Alpha);
	sm.ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
	sm.m_StandardShader.DrawPolygon(m_poly, world, kWhiteColor);
	sm.UndoDepthStencilState();
	sm.UndoBlendState();
	sm.UndoSamplerState();
}
