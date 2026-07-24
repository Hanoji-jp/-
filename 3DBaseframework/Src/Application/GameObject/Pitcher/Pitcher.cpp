#include "Pitcher.h"
#include "PitcherConst.h"
#include <cmath>

void Pitcher::Init()
{
	m_drawType = eDrawTypeLit;
	BuildMesh();
}

// ===================================================
// プロファイル（半径r, 高さy）を Y軸回りに回転させ、ジャグの側面＋底を
// 三角形ストリップで作る。各頂点に法線（側面=外向き／底=下向き）。
// ===================================================
void Pitcher::BuildMesh()
{
	using Vertex = KdPolygon::Vertex;

	const std::vector<Math::Vector2>& prof = PitcherConst::kProfile;	// (x=半径, y=高さ)
	if (prof.size() < 2) { return; }

	const int N = PitcherConst::kSegments;
	const int R = PitcherConst::kRings;

	const float yBottom = prof.front().y;
	const float yTop    = prof.back().y;

	// 高さ y における半径（プロファイルの区分線形補間）
	auto radiusAtY = [&](float y) -> float
	{
		if (y <= prof.front().y) { return prof.front().x; }
		if (y >= prof.back().y)  { return prof.back().x; }
		for (size_t i = 0; i + 1 < prof.size(); ++i)
		{
			if (y <= prof[i + 1].y)
			{
				const float t = (y - prof[i].y) / (prof[i + 1].y - prof[i].y);
				return prof[i].x + (prof[i + 1].x - prof[i].x) * t;
			}
		}
		return prof.back().x;
	};

	struct Ring { float y, r, nr, ny; };

	auto makeV = [&](float y, float r, float nr, float ny, int s) -> Vertex
	{
		const float ang = 6.28318530718f * static_cast<float>(s) / static_cast<float>(N);
		const float c = std::cos(ang);
		const float sn = std::sin(ang);
		Vertex v;
		v.pos    = { r * c, y, r * sn };
		v.normal = { nr * c, ny, nr * sn };
		v.UV     = { static_cast<float>(s) / static_cast<float>(N), 0.0f };
		return v;
	};

	std::vector<Vertex> verts;

	auto appendStrip = [&](const std::vector<Ring>& rings)
	{
		const int bands = static_cast<int>(rings.size()) - 1;
		for (int j = 0; j < bands; ++j)
		{
			if (!verts.empty())
			{
				verts.push_back(verts.back());
				verts.push_back(makeV(rings[j].y, rings[j].r, rings[j].nr, rings[j].ny, 0));
			}
			for (int s = 0; s <= N; ++s)
			{
				verts.push_back(makeV(rings[j].y,     rings[j].r,     rings[j].nr,     rings[j].ny,     s));
				verts.push_back(makeV(rings[j + 1].y, rings[j + 1].r, rings[j + 1].nr, rings[j + 1].ny, s));
			}
		}
	};

	// --- 底キャップ（半径 0→底半径、下向き法線） ---
	const float rBottom = prof.front().x;
	const int   RC = 3;
	std::vector<Ring> cap(static_cast<size_t>(RC) + 1);
	for (int k = 0; k <= RC; ++k)
	{
		const float rr = rBottom * (static_cast<float>(k) / static_cast<float>(RC));
		cap[k] = { yBottom, rr, 0.0f, -1.0f };	// 法線=下
	}
	appendStrip(cap);

	// --- 側面（底～口、外向き法線） ---
	std::vector<Ring> side(static_cast<size_t>(R) + 1);
	for (int j = 0; j <= R; ++j)
	{
		const float t = static_cast<float>(j) / static_cast<float>(R);
		const float y = yBottom + (yTop - yBottom) * t;
		side[j].y = y;
		side[j].r = radiusAtY(y);
	}
	for (int j = 0; j <= R; ++j)
	{
		const int j0 = (j > 0) ? j - 1 : 0;
		const int j1 = (j < R) ? j + 1 : R;
		const float dy = side[j1].y - side[j0].y;
		const float dr = side[j1].r - side[j0].r;
		float len = std::sqrt(dy * dy + dr * dr);
		if (len < 1e-6f) { len = 1.0f; }
		side[j].nr =  dy / len;
		side[j].ny = -dr / len;
	}
	appendStrip(side);

	m_poly.SetVertices(verts);
}

void Pitcher::DrawLit()
{
	// 濃い茶色の不透明ジャグ。ライティングありの通常描画。
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_poly, PitcherConst::WorldMatrix(), PitcherConst::kColor);
}
