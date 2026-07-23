#include "Cup.h"
#include "CupConst.h"
#include <cmath>

void Cup::Init()
{
	m_drawType = eDrawTypeLit;

	// ガラスの回転体メッシュを1回だけ生成
	BuildGlassMesh();
}

// ===================================================
// kInnerShape の右半分（x>0）を半径 r(y) として Y軸回りに回転させ、
// ガラスの側面を三角形ストリップで作る。各頂点に外向き法線を持たせる。
// ===================================================
void Cup::BuildGlassMesh()
{
	using Vertex = KdPolygon::Vertex;

	// --- 右半分の輪郭（半径=x, 高さ=y）を取り出して高さ順に並べる ---
	std::vector<Math::Vector2> prof;	// (x=半径, y=高さ)
	for (const Math::Vector2& p : CupConst::kInnerShape)
	{
		if (p.x > 0.0f) { prof.push_back(p); }
	}
	std::sort(prof.begin(), prof.end(),
		[](const Math::Vector2& a, const Math::Vector2& b) { return a.y < b.y; });
	if (prof.size() < 2) { return; }

	const float yBottom = prof.front().y;
	const float yTop    = prof.back().y;

	// 高さ y における半径（輪郭の区分線形補間）＋肉厚
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

	const int N = CupConst::kGlassSegments;	// 回転分割
	const int R = CupConst::kGlassRings;		// 高さ分割

	// --- 各リング（高さ・半径・輪郭法線）を用意 ---
	struct Ring { float y, r, nr, ny; };
	std::vector<Ring> rings(static_cast<size_t>(R) + 1);
	for (int j = 0; j <= R; ++j)
	{
		const float t = static_cast<float>(j) / static_cast<float>(R);
		const float y = yBottom + (yTop - yBottom) * t;
		rings[j].y = y;
		rings[j].r = radiusAtY(y) + CupConst::kGlassThickness;
	}
	// 輪郭の傾きから外向き法線 (nr, ny) を求める（(r,y)平面で接線に直交）
	for (int j = 0; j <= R; ++j)
	{
		const int j0 = (j > 0) ? j - 1 : 0;
		const int j1 = (j < R) ? j + 1 : R;
		const float dy = rings[j1].y - rings[j0].y;
		const float dr = rings[j1].r - rings[j0].r;
		float len = std::sqrt(dy * dy + dr * dr);
		if (len < 1e-6f) { len = 1.0f; }
		rings[j].nr =  dy / len;	// 半径方向（外向き）成分
		rings[j].ny = -dr / len;
	}

	// リングjの角度sの頂点を作る
	auto makeV = [&](int j, int s) -> Vertex
	{
		const float ang = 6.28318530718f * static_cast<float>(s) / static_cast<float>(N);
		const float c = std::cos(ang);
		const float sn = std::sin(ang);
		Vertex v;
		v.pos    = { rings[j].r * c, rings[j].y, rings[j].r * sn };
		v.normal = { rings[j].nr * c, rings[j].ny, rings[j].nr * sn };
		v.UV     = { static_cast<float>(s) / static_cast<float>(N),
					 static_cast<float>(j) / static_cast<float>(R) };
		return v;
	};

	// --- 三角形ストリップで側面を張る（リング帯ごとに、帯間は縮退頂点で繋ぐ）---
	std::vector<Vertex> verts;
	verts.reserve(static_cast<size_t>(R) * (N + 1) * 2 + R * 2);
	for (int j = 0; j < R; ++j)
	{
		for (int s = 0; s <= N; ++s)
		{
			verts.push_back(makeV(j, s));		// 下リング
			verts.push_back(makeV(j + 1, s));	// 上リング
		}
		if (j < R - 1)
		{
			// 帯の切れ目：直前の頂点と次の帯の先頭を重ねて縮退三角形にする
			verts.push_back(makeV(j + 1, N));
			verts.push_back(makeV(j + 1, 0));
		}
	}

	m_glassPoly.SetVertices(verts);
}

void Cup::DrawLit()
{
	// 目標ラインは水・ガラスより後（前面）に描きたいので DrawEffect 側で描く。
}

void Cup::DrawEffect()
{
	auto& sm = KdShaderManager::Instance();

	// 半透明の3Dガラス。不透明シーン（机）＋水の後に描かれる（オブジェクト順で Cup は Water の後）。
	//  ・Alphaブレンドで半透明、ZWriteDisableで奥壁も見える（DrawPolygonは内部CullNoneで両壁描画）。
	sm.ChangeBlendState(KdBlendState::Alpha);
	sm.ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
	sm.m_StandardShader.DrawPolygon(m_glassPoly, CupConst::PlacementMatrix(), CupConst::kGlassColor);
	sm.UndoDepthStencilState();
	sm.UndoBlendState();

	// 目標ライン（この線ぴったりを狙う）。水・ガラスの後に描くことで常に前面に見える。
	//  DrawVertices は内部で深度無効なので、奥行きに関係なく最前面へ出る。
	std::vector<KdPolygon::Vertex> line(2);
	line[0].pos = { CupConst::kInnerLeftX,  CupConst::kTargetLineY, 0.0f };
	line[1].pos = { CupConst::kInnerRightX, CupConst::kTargetLineY, 0.0f };
	sm.m_StandardShader.DrawVertices(line, CupConst::PlacementMatrix(), CupConst::kLineColor);
}
