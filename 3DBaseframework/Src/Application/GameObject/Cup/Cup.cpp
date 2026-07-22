#include "Cup.h"
#include "CupConst.h"

void Cup::Init()
{
	m_drawType = eDrawTypeLit;
}

void Cup::DrawLit()
{
	auto& standardShader = KdShaderManager::Instance().m_StandardShader;

	const float leftX  = CupConst::kInnerLeftX;
	const float rightX = CupConst::kInnerRightX;
	const float botY   = CupConst::kInnerBottomY;
	const float topY   = CupConst::kInnerTopY;

	// コップの枠（左壁・底・右壁）：LINELIST（2頂点で1本）
	std::vector<KdPolygon::Vertex> wall(6);
	wall[0].pos = { leftX,  topY, 0.0f };	wall[1].pos = { leftX,  botY, 0.0f };	// 左壁
	wall[2].pos = { leftX,  botY, 0.0f };	wall[3].pos = { rightX, botY, 0.0f };	// 底
	wall[4].pos = { rightX, botY, 0.0f };	wall[5].pos = { rightX, topY, 0.0f };	// 右壁
	standardShader.DrawVertices(wall, Math::Matrix::Identity, CupConst::kWallColor);

	// 目標ライン（この線ぴったりを狙う）
	std::vector<KdPolygon::Vertex> line(2);
	line[0].pos = { leftX,  CupConst::kTargetLineY, 0.0f };
	line[1].pos = { rightX, CupConst::kTargetLineY, 0.0f };
	standardShader.DrawVertices(line, Math::Matrix::Identity, CupConst::kLineColor);
}
