#include "Water.h"
#include "WaterConst.h"
#include "../Cup/CupConst.h"
#include "../../main.h"

namespace
{
	// Math::Color を頂点カラー(RGBA/8bit)へ変換する
	unsigned int PackColor(const Math::Color& c)
	{
		unsigned char r = static_cast<unsigned char>(c.R() * 255);
		unsigned char g = static_cast<unsigned char>(c.G() * 255);
		unsigned char b = static_cast<unsigned char>(c.B() * 255);
		unsigned char a = static_cast<unsigned char>(c.A() * 255);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

// ===================================================
// 水面板ポリゴン：高さ配列から三角形ストリップを組む
// ===================================================
void WaterSurfacePolygon::UpdateSurface(const std::vector<float>& heights, float leftX, float columnStride, float bottomY)
{
	const size_t count = heights.size();
	if (count < 2) { return; }

	const unsigned int topColor = PackColor(WaterConst::kColorTop);
	const unsigned int botColor = PackColor(WaterConst::kColorBottom);

	// 1列につき「底の頂点」「水面の頂点」の2点を並べてストリップにする
	m_vertices.resize(count * 2);
	for (size_t i = 0; i < count; ++i)
	{
		const float x    = leftX + columnStride * static_cast<float>(i);
		const float topY = bottomY + heights[i];

		Vertex& bottomVtx = m_vertices[i * 2 + 0];
		bottomVtx.pos   = { x, bottomY, 0.0f };
		bottomVtx.UV    = { 0.0f, 1.0f };
		bottomVtx.color = botColor;

		Vertex& topVtx = m_vertices[i * 2 + 1];
		topVtx.pos   = { x, topY, 0.0f };
		topVtx.UV    = { 0.0f, 0.0f };
		topVtx.color = topColor;
	}
}

// ===================================================
// 水本体
// ===================================================
void Water::Init()
{
	// 開始時に少しだけ水を入れておく（描画確認しやすいように）
	const float initialHeight = CupConst::kInnerHeight * WaterConst::kInitialFillRate;
	m_heights.assign(WaterConst::kColumnCount, initialHeight);
	m_velocities.assign(WaterConst::kColumnCount, 0.0f);
	m_fillRate  = WaterConst::kInitialFillRate;
	m_isPouring = false;

	m_drawType = eDrawTypeLit;
}

void Water::Reset()
{
	std::fill(m_heights.begin(),    m_heights.end(),    0.0f);
	std::fill(m_velocities.begin(), m_velocities.end(), 0.0f);
	m_fillRate = 0.0f;
}

void Water::Update()
{
	float deltaTime = Application::Instance().GetDeltaTime();
	if (deltaTime > WaterConst::kMaxDeltaTime) { deltaTime = WaterConst::kMaxDeltaTime; }

	// 注水：中央付近の列に水を足す
	if (m_isPouring)
	{
		const int center = WaterConst::kColumnCount / 2;
		for (int offset = -WaterConst::kPourHalfWidth; offset <= WaterConst::kPourHalfWidth; ++offset)
		{
			const int index = center + offset;
			if (index < 0 || index >= WaterConst::kColumnCount) { continue; }
			m_heights[index] += WaterConst::kPourRisePerSec * deltaTime;
		}
	}

	// 波の伝播を進める
	StepSimulation(deltaTime);

	// 高さを範囲内に収めつつ、平均水位を計算する
	float sum = 0.0f;
	for (float& h : m_heights)
	{
		h = std::clamp(h, 0.0f, CupConst::kInnerHeight);
		sum += h;
	}
	const float average = sum / static_cast<float>(WaterConst::kColumnCount);
	m_fillRate = average / CupConst::kInnerHeight;
}

void Water::StepSimulation(float deltaTime)
{
	const int n = WaterConst::kColumnCount;

	// 隣接列との高さ差から速度を更新（両端は自分自身を隣とみなす＝壁で反射）
	for (int i = 0; i < n; ++i)
	{
		const float heightLeft  = (i > 0)     ? m_heights[i - 1] : m_heights[i];
		const float heightRight = (i < n - 1) ? m_heights[i + 1] : m_heights[i];

		const float force = WaterConst::kWaveSpread *
			((heightLeft - m_heights[i]) + (heightRight - m_heights[i]));

		m_velocities[i] += force * deltaTime;
		m_velocities[i] *= std::max(0.0f, 1.0f - WaterConst::kVelocityDamp * deltaTime);
	}

	// 速度で高さを更新
	for (int i = 0; i < n; ++i)
	{
		m_heights[i] += m_velocities[i] * deltaTime;
	}
}

void Water::DrawLit()
{
	if (m_heights.size() < 2) { return; }

	// 高さ配列から水面ポリゴンを更新
	const float stride = CupConst::kInnerWidth / static_cast<float>(WaterConst::kColumnCount - 1);
	m_surface.UpdateSurface(m_heights, CupConst::kInnerLeftX, stride, CupConst::kInnerBottomY);

	// 陰影で暗くならないよう、水色を自己発光として与える
	const Math::Vector3 emissive =
	{
		WaterConst::kColorTop.R() * WaterConst::kEmissiveRate,
		WaterConst::kColorTop.G() * WaterConst::kEmissiveRate,
		WaterConst::kColorTop.B() * WaterConst::kEmissiveRate,
	};

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(
		m_surface, Math::Matrix::Identity, kWhiteColor, emissive);
}
