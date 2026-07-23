#include "Cup.h"
#include "CupConst.h"

void Cup::Init()
{
	m_drawType = eDrawTypeLit;
}

void Cup::DrawLit()
{
	auto& standardShader = KdShaderManager::Instance().m_StandardShader;

	// コップ内側の輪郭（自由多角形）を線で描く。
	//  LINELIST（2頂点で1本）で連続する頂点を結ぶ。最後→最初（開口）は結ばない。
	const std::vector<Math::Vector2>& shape = CupConst::kInnerShape;
	const size_t n = shape.size();
	if (n >= 2)
	{
		std::vector<KdPolygon::Vertex> wall;
		wall.reserve((n - 1) * 2);
		for (size_t i = 0; i + 1 < n; ++i)
		{
			KdPolygon::Vertex v0, v1;
			v0.pos = { shape[i].x,     shape[i].y,     0.0f };
			v1.pos = { shape[i + 1].x, shape[i + 1].y, 0.0f };
			wall.push_back(v0);
			wall.push_back(v1);
		}
		standardShader.DrawVertices(wall, Math::Matrix::Identity, CupConst::kWallColor);
	}

	// 目標ライン（この線ぴったりを狙う）
	std::vector<KdPolygon::Vertex> line(2);
	line[0].pos = { CupConst::kInnerLeftX,  CupConst::kTargetLineY, 0.0f };
	line[1].pos = { CupConst::kInnerRightX, CupConst::kTargetLineY, 0.0f };
	standardShader.DrawVertices(line, Math::Matrix::Identity, CupConst::kLineColor);
}
