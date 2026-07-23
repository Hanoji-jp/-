#include "FluidField.h"
#include "WaterConst.h"

// ===================================================
// 初期化：シェーダ・レンダーターゲット・初期データの生成
// ===================================================
bool FluidField::Init()
{
	ID3D11Device* dev = KdDirect3D::Instance().WorkDev();
	if (!dev) { return false; }

	// -------- フルスクリーン頂点（クリップ空間 / トライアングルストリップ） --------
	//  WaterSurfacePolygon と同じ「下→上」「左→右」順で並べる
	m_screenVert[0].Pos = { -1.0f, -1.0f, 0.0f };	// 左下
	m_screenVert[1].Pos = { -1.0f,  1.0f, 0.0f };	// 左上
	m_screenVert[2].Pos = {  1.0f, -1.0f, 0.0f };	// 右下
	m_screenVert[3].Pos = {  1.0f,  1.0f, 0.0f };	// 右上

	// -------- 頂点シェーダ＋入力レイアウト --------
	{
#include "../../Shader/Fluid/FluidShader_VS_FullScreen.shaderInc"

		if (FAILED(dev->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_vsFullScreen.GetAddressOf())))
		{
			assert(0 && "流体VSの作成に失敗");
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		if (FAILED(dev->CreateInputLayout(layout, _countof(layout), compiledBuffer, sizeof(compiledBuffer), m_inputLayout.GetAddressOf())))
		{
			assert(0 && "流体 入力レイアウトの作成に失敗");
			return false;
		}
	}

	// -------- 恒等コピーPS --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Copy.shaderInc"

		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psCopy.GetAddressOf())))
		{
			assert(0 && "流体 コピーPSの作成に失敗");
			return false;
		}
	}

	// -------- 可視化PS --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Visualize.shaderInc"

		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psVisualize.GetAddressOf())))
		{
			assert(0 && "流体 可視化PSの作成に失敗");
			return false;
		}
	}

	// -------- 表面（体積フラックス）PS --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Surface.shaderInc"

		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psSurface.GetAddressOf())))
		{
			assert(0 && "流体 表面PSの作成に失敗");
			return false;
		}
	}

	// -------- 移流VS＋入力レイアウト --------
	{
#include "../../Shader/Fluid/FluidShader_VS_Advection.shaderInc"

		if (FAILED(dev->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_vsAdvection.GetAddressOf())))
		{
			assert(0 && "流体 移流VSの作成に失敗");
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		if (FAILED(dev->CreateInputLayout(layout, _countof(layout), compiledBuffer, sizeof(compiledBuffer), m_advectionLayout.GetAddressOf())))
		{
			assert(0 && "流体 移流入力レイアウトの作成に失敗");
			return false;
		}
	}

	// -------- 移流GS（点→クアッド） --------
	{
#include "../../Shader/Fluid/FluidShader_GS_Advection.shaderInc"

		if (FAILED(dev->CreateGeometryShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_gsAdvection.GetAddressOf())))
		{
			assert(0 && "流体 移流GSの作成に失敗");
			return false;
		}
	}

	// -------- 移流PS（重み付き撒き込み） --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Advection.shaderInc"

		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psAdvection.GetAddressOf())))
		{
			assert(0 && "流体 移流PSの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：compression --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Compression.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psCompression.GetAddressOf())))
		{
			assert(0 && "流体 compression PSの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：downsample --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Downsample.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psDownsample.GetAddressOf())))
		{
			assert(0 && "流体 downsample PSの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：upsample --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Upsample.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psUpsample.GetAddressOf())))
		{
			assert(0 && "流体 upsample PSの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：solution（ヤコビ反復） --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Solution.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psSolution.GetAddressOf())))
		{
			assert(0 && "流体 solution PSの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：pressure-gradient --------
	{
#include "../../Shader/Fluid/FluidShader_PS_PressureGradient.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psPressureGradient.GetAddressOf())))
		{
			assert(0 && "流体 pressure-gradient PSの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：pressure-acceleration --------
	{
#include "../../Shader/Fluid/FluidShader_PS_PressureAcceleration.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psPressureAcceleration.GetAddressOf())))
		{
			assert(0 && "流体 pressure-acceleration PSの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：pressure-displacement --------
	{
#include "../../Shader/Fluid/FluidShader_PS_PressureDisplacement.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psPressureDisplacement.GetAddressOf())))
		{
			assert(0 && "流体 pressure-displacement PSの作成に失敗");
			return false;
		}
	}

	// -------- 注水PS --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Pour.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psPour.GetAddressOf())))
		{
			assert(0 && "流体 注水PSの作成に失敗");
			return false;
		}
	}

	// -------- 補填PS --------
	{
#include "../../Shader/Fluid/FluidShader_PS_Equalization.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psEqualization.GetAddressOf())))
		{
			assert(0 && "流体 補填PSの作成に失敗");
			return false;
		}
	}

	// -------- 炭酸：泡PS群 --------
	{
#include "../../Shader/Fluid/FluidShader_PS_FoamGen.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psFoamGen.GetAddressOf())))
		{
			assert(0 && "流体 泡発生PSの作成に失敗");
			return false;
		}
	}
	{
#include "../../Shader/Fluid/FluidShader_PS_FoamLoss.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psFoamLoss.GetAddressOf())))
		{
			assert(0 && "流体 泡質量減算PSの作成に失敗");
			return false;
		}
	}
	{
#include "../../Shader/Fluid/FluidShader_PS_FoamBuoyancy.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psFoamBuoyancy.GetAddressOf())))
		{
			assert(0 && "流体 泡浮上PSの作成に失敗");
			return false;
		}
	}
	{
#include "../../Shader/Fluid/FluidShader_PS_FoamToLiquid.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psFoamToLiquid.GetAddressOf())))
		{
			assert(0 && "流体 泡→液PSの作成に失敗");
			return false;
		}
	}
	{
#include "../../Shader/Fluid/FluidShader_PS_FoamDecay.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psFoamDecay.GetAddressOf())))
		{
			assert(0 && "流体 泡減衰PSの作成に失敗");
			return false;
		}
	}

	// -------- 質量正規化PS --------
	{
#include "../../Shader/Fluid/FluidShader_PS_MassScale.shaderInc"
		if (FAILED(dev->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_psMassScale.GetAddressOf())))
		{
			assert(0 && "流体 質量正規化PSの作成に失敗");
			return false;
		}
	}

	// -------- 定数バッファ（可視化色） --------
	{
		cbVisualize init;
		const Math::Color& w = WaterConst::kVisualizeWaterColor;
		const Math::Color& s = WaterConst::kVisualizeSpaceColor;
		const Math::Color& f = WaterConst::kVisualizeFoamColor;
		init.WaterColor = { w.R(), w.G(), w.B(), w.A() };
		init.SpaceColor = { s.R(), s.G(), s.B(), s.A() };
		init.FoamColor  = { f.R(), f.G(), f.B(), f.A() };
		init.FoamParams = { WaterConst::kVisualizeFoamGain, WaterConst::kVisualizeWaterLo, WaterConst::kVisualizeWaterHi, 0.0f };
		m_cbVisualize.Create(&init);
	}

	// -------- 炭酸：泡の定数バッファ＋フィールド --------
	{
		cbFoam f;
		f.GenRate     = WaterConst::kFoamGenRate;
		f.SpeedThresh = WaterConst::kFoamSpeedThresh;
		f.MassThresh  = WaterConst::kFoamMassThresh;
		f.RiseRate    = WaterConst::kFoamRiseRate;
		f.SpreadRate  = WaterConst::kFoamSpreadRate;
		f.DecayRate   = WaterConst::kFoamDecayRate;
		m_cbFoam.Create(&f);

		// 泡フィールド（0で初期化）
		std::vector<Math::Vector4> zero(static_cast<size_t>(WaterConst::kGridWidth) * WaterConst::kGridHeight,
			Math::Vector4(0.0f, 0.0f, 0.0f, 0.0f));
		D3D11_SUBRESOURCE_DATA fsrd = {};
		fsrd.pSysMem = zero.data();
		fsrd.SysMemPitch = static_cast<UINT>(WaterConst::kGridWidth * sizeof(Math::Vector4));

		m_foam.current = std::make_shared<KdTexture>();
		m_foam.next    = std::make_shared<KdTexture>();
		if (!m_foam.current->CreateRenderTarget(WaterConst::kGridWidth, WaterConst::kGridHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, &fsrd) ||
			!m_foam.next->CreateRenderTarget(WaterConst::kGridWidth, WaterConst::kGridHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, nullptr))
		{
			assert(0 && "流体 泡フィールドの作成に失敗");
			return false;
		}
	}

	// -------- レンダーターゲット群 --------
	std::vector<Math::Vector4> quantityData;
	BuildInitialQuantity(quantityData);

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = quantityData.data();
	srd.SysMemPitch = static_cast<UINT>(WaterConst::kGridWidth * sizeof(Math::Vector4));

	m_quantity.current = std::make_shared<KdTexture>();
	m_quantity.next = std::make_shared<KdTexture>();
	m_displayTex = std::make_shared<KdTexture>();

	if (!m_quantity.current->CreateRenderTarget(WaterConst::kGridWidth, WaterConst::kGridHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, &srd) ||
		!m_quantity.next->CreateRenderTarget(WaterConst::kGridWidth, WaterConst::kGridHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, nullptr) ||
		!m_displayTex->CreateRenderTarget(WaterConst::kGridWidth, WaterConst::kGridHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr))
	{
		assert(0 && "流体 レンダーターゲットの作成に失敗");
		return false;
	}

	// -------- 定数バッファ（重力・グリッドサイズ） --------
	{
		cbAdvection init;
		init.Gravity  = { 0.0f, WaterConst::kGravityPerStep };	// row増加＝画面下が +
		init.GridSize = { static_cast<float>(WaterConst::kGridWidth), static_cast<float>(WaterConst::kGridHeight) };
		m_cbAdvection.Create(&init);
	}

	// -------- 純加算ブレンド(ONE,ONE)：移流の撒き込み用 --------
	//  KdBlendState::Add は SRC_ALPHA,ONE なので流体には使えず、専用に作る
	{
		D3D11_BLEND_DESC bd = {};
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend       = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlend      = D3D11_BLEND_ONE;
		bd.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		if (FAILED(dev->CreateBlendState(&bd, m_additiveBlend.GetAddressOf())))
		{
			assert(0 && "流体 加算ブレンドの作成に失敗");
			return false;
		}
	}

	// -------- 移流用 頂点バッファ（1セル1点：セル中心のピクセル座標） --------
	{
		std::vector<CellVertex> cells;
		cells.reserve(static_cast<size_t>(WaterConst::kGridWidth) * WaterConst::kGridHeight);
		for (int row = 0; row < WaterConst::kGridHeight; ++row)
		{
			for (int col = 0; col < WaterConst::kGridWidth; ++col)
			{
				cells.push_back({ { col + 0.5f, row + 0.5f } });
			}
		}

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = static_cast<UINT>(cells.size() * sizeof(CellVertex));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA init = {};
		init.pSysMem = cells.data();
		if (FAILED(dev->CreateBuffer(&bd, &init, m_cellPosVB.GetAddressOf())))
		{
			assert(0 && "流体 セル頂点バッファの作成に失敗");
			return false;
		}
	}

	// -------- 圧力：intensityピラミッド（W×H から半分ずつ、最小サイズまで） --------
	for (int lw = WaterConst::kGridWidth, lh = WaterConst::kGridHeight;
		lw >= WaterConst::kMinPyramidSize && lh >= WaterConst::kMinPyramidSize;
		lw >>= 1, lh >>= 1)
	{
		DoubleBuffer level;
		level.current = std::make_shared<KdTexture>();
		level.next    = std::make_shared<KdTexture>();
		if (!level.current->CreateRenderTarget(lw, lh, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, nullptr) ||
			!level.next->CreateRenderTarget(lw, lh, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, nullptr))
		{
			assert(0 && "流体 intensityバッファの作成に失敗");
			return false;
		}
		m_intensity.push_back(level);
	}

	// -------- 圧力：connection（4近傍への圧力勾配） --------
	m_connection = std::make_shared<KdTexture>();
	if (!m_connection->CreateRenderTarget(WaterConst::kGridWidth, WaterConst::kGridHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, nullptr))
	{
		assert(0 && "流体 connectionの作成に失敗");
		return false;
	}

	// -------- 注水・補填の定数バッファ＋初期水量 --------
	{
		// 吸い込み口（上部中央の矩形）
		const int centerCol = WaterConst::kGridWidth / 2;
		const int colMin = centerCol - WaterConst::kPourHalfCols;
		const int colMax = centerCol + WaterConst::kPourHalfCols;
		const int rowMin = WaterConst::kPourTopRow;
		const int rowMax = WaterConst::kPourTopRow + WaterConst::kPourRows;
		m_inletCellCount = (colMax - colMin) * (rowMax - rowMin);

		cbPour p;
		p.InletRect = { static_cast<float>(colMin), static_cast<float>(rowMin),
						static_cast<float>(colMax), static_cast<float>(rowMax) };
		p.MassRate = WaterConst::kPourMassPerStep;
		p.Velocity = WaterConst::kPourVelocity;
		m_cbPour.Create(&p);

		// 初期水量（質量の合計）を求める
		std::vector<Math::Vector4> initData;
		BuildInitialQuantity(initData);
		float sumMass = 0.0f;
		for (const Math::Vector4& q : initData) { sumMass += q.z; }

		cbEqualization e;
		e.SurplusRate = WaterConst::kEqualizationSurplusRate;
		e.DeficitRate = WaterConst::kEqualizationDeficitRate;
		// 不足セルは「水の密度」で補充（拡散で薄まった水を保ち、水量が減らない）
		e.DeficitRatio = { 0.0f, 0.0f, WaterConst::kEqualizationRefillDensity, 1.0f };
		m_cbEqualization.Create(&e);

		m_waterMass = sumMass;	// 現在の水量（以後、注水ぶんを加算していく）
	}

	// -------- 質量正規化：読み戻し用ステージングテクスチャ＋CB --------
	{
		D3D11_TEXTURE2D_DESC td = {};
		td.Width = WaterConst::kGridWidth;
		td.Height = WaterConst::kGridHeight;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		td.SampleDesc.Count = 1;
		td.Usage = D3D11_USAGE_STAGING;
		td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		if (FAILED(dev->CreateTexture2D(&td, nullptr, m_massReadback.GetAddressOf())))
		{
			assert(0 && "流体 質量読み戻しテクスチャの作成に失敗");
			return false;
		}

		cbMassScale ms;
		ms.Scale = 1.0f;
		m_cbMassScale.Create(&ms);
	}

	m_initialized = true;
	return true;
}

// ===================================================
// 1ステップ進める
//  表面 → 移流＋重力 → 圧力解法 → 注水 → 補填 → 可視化
//  ※重力・注水はHTMLに倣い固定ステップ量（deltaTimeでスケールしない）
// ===================================================
void FluidField::Step(float deltaTime, bool pouring)
{
	if (!m_initialized) { return; }
	(void)deltaTime;

	KdShaderManager& sm = KdShaderManager::Instance();

	// フルスクリーン／撒き込みクアッドが裏面カリングされないようにする
	sm.ChangeRasterizerState(KdRasterizerState::CullNone);

	// ① 表面（自由表面：体積フラックス）
	RenderPass(m_psSurface.Get(), m_quantity.current, m_quantity.next);
	m_quantity.Swap();

	// ② 移流＋重力（1セル1点を GS で撒き込み、純加算で配分）
	RenderAdvection(m_quantity.current, m_quantity.next);
	m_quantity.Swap();

	// ③ 圧力解法（非圧縮化：マルチグリッド＋加速・変位）
	SolvePressure();

	// ④ 注水（SPACE押下中）：上部中央から水を加える
	if (pouring)
	{
		m_cbPour.Write();
		sm.SetPSConstantBuffer(0, m_cbPour.GetAddress());
		RenderPass(m_psPour.Get(), m_quantity.current, m_quantity.next);
		m_quantity.Swap();

		// 注いだぶんを水量へ積算（水位判定用。読み戻し不要）
		m_waterMass += WaterConst::kPourMassPerStep * static_cast<float>(m_inletCellCount);
	}

	// ⑤ 補填（余剰/不足の是正で水面を安定させる）
	//  非保存的に質量を増減するため既定では無効（水が減るのを防ぐ）。
	if (WaterConst::kEnableEqualization)
	{
		m_cbEqualization.Write();
		sm.SetPSConstantBuffer(0, m_cbEqualization.GetAddress());
		RenderPass(m_psEqualization.Get(), m_quantity.current, m_quantity.next);
		m_quantity.Swap();
	}

	// ⑥ 炭酸：泡（発生・浮上・拡散・減衰＝液へ還元）
	//  無効時（普通の水）は泡パスをスキップ。foamフィールドは0のまま＝泡は描かれない。
	if (WaterConst::kEnableCarbonation)
	{
		SolveFoam();
	}

	// ⑦ 質量正規化：GPU総質量を「注いだ量」へ合わせ、ドリフトで水が減るのを防ぐ
	NormalizeMass();

	// ⑧ 現在の物理量＋泡から表示用テクスチャを生成
	m_cbVisualize.Write();
	sm.SetPSConstantBuffer(0, m_cbVisualize.GetAddress());
	RenderPass2(m_psVisualize.Get(), m_quantity.current, m_foam.current, m_displayTex);

	sm.UndoRasterizerState();

	// 次フレームの測定用に、確定した物理量をステージングへコピーしておく
	KdDirect3D::Instance().WorkDevContext()->CopyResource(m_massReadback.Get(), m_quantity.current->WorkResource());
	m_readbackValid = true;
}

// ===================================================
// 質量正規化：前フレームのコピーから総質量を測り、注いだ量へ合わせてスケール
// ===================================================
void FluidField::NormalizeMass()
{
	ID3D11DeviceContext* ctx = KdDirect3D::Instance().WorkDevContext();

	// 前フレームにコピー済みなら、総質量を測ってスケール率を更新する（読み戻しは1フレーム遅れ＝ストール無し）
	if (m_readbackValid)
	{
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		if (SUCCEEDED(ctx->Map(m_massReadback.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
		{
			const int w = WaterConst::kGridWidth;
			const int h = WaterConst::kGridHeight;
			double sum = 0.0;
			const char* base = static_cast<const char*>(mapped.pData);
			for (int r = 0; r < h; ++r)
			{
				const Math::Vector4* row = reinterpret_cast<const Math::Vector4*>(base + static_cast<size_t>(r) * mapped.RowPitch);
				for (int c = 0; c < w; ++c) { sum += row[c].z; }
			}
			ctx->Unmap(m_massReadback.Get(), 0);

			if (sum > 1e-4)
			{
				const float scale = m_waterMass / static_cast<float>(sum);
				m_massScale = std::clamp(scale, 0.8f, 1.25f);	// 極端な補正は避ける
			}
		}
	}

	// スケールを質量・運動量へ一律適用
	m_cbMassScale.Work().Scale = m_massScale;
	m_cbMassScale.Write();
	KdShaderManager::Instance().SetPSConstantBuffer(0, m_cbMassScale.GetAddress());
	RenderPass(m_psMassScale.Get(), m_quantity.current, m_quantity.next);
	m_quantity.Swap();
}

// ===================================================
// 現在の水位（0.0=空 ～ 1.0=満杯）
//  満杯 = 全セルが水（質量1）＝ W*H。注いだ水量から算出する。
// ===================================================
float FluidField::GetFillRate() const
{
	const float capacity = static_cast<float>(WaterConst::kGridWidth) *
						   static_cast<float>(WaterConst::kGridHeight) * WaterConst::kCellWaterMass;
	if (capacity <= 0.0f) { return 0.0f; }
	return std::clamp(m_waterMass / capacity, 0.0f, 1.0f);
}

// ===================================================
// 初期状態へ戻す
// ===================================================
void FluidField::Reset()
{
	if (!m_initialized) { return; }

	std::vector<Math::Vector4> quantityData;
	BuildInitialQuantity(quantityData);

	// current バッファの内容を初期データで上書き（RTは DEFAULT usage なので UpdateSubresource 可）
	ID3D11DeviceContext* ctx = KdDirect3D::Instance().WorkDevContext();
	const UINT rowPitch = static_cast<UINT>(WaterConst::kGridWidth * sizeof(Math::Vector4));
	ctx->UpdateSubresource(m_quantity.current->WorkResource(), 0, nullptr, quantityData.data(), rowPitch, 0);

	// 水量を初期値に戻す
	float sumMass = 0.0f;
	for (const Math::Vector4& q : quantityData) { sumMass += q.z; }
	m_waterMass = sumMass;

	// 泡フィールドを0クリア
	std::vector<Math::Vector4> zero(quantityData.size(), Math::Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	ctx->UpdateSubresource(m_foam.current->WorkResource(), 0, nullptr, zero.data(), rowPitch, 0);

	// 質量正規化の状態を初期化（古いコピーで誤補正しないように）
	m_readbackValid = false;
	m_massScale = 1.0f;
}

// ===================================================
// 解放
// ===================================================
void FluidField::Release()
{
	m_cbVisualize.Release();
	m_cbAdvection.Release();

	m_vsFullScreen.Reset();
	m_inputLayout.Reset();
	m_psCopy.Reset();
	m_psVisualize.Reset();
	m_psSurface.Reset();

	m_vsAdvection.Reset();
	m_gsAdvection.Reset();
	m_psAdvection.Reset();
	m_advectionLayout.Reset();
	m_cellPosVB.Reset();
	m_additiveBlend.Reset();

	m_psCompression.Reset();
	m_psDownsample.Reset();
	m_psUpsample.Reset();
	m_psSolution.Reset();
	m_psPressureGradient.Reset();
	m_psPressureAcceleration.Reset();
	m_psPressureDisplacement.Reset();

	m_psPour.Reset();
	m_psEqualization.Reset();
	m_cbPour.Release();
	m_cbEqualization.Release();

	m_psFoamGen.Reset();
	m_psFoamLoss.Reset();
	m_psFoamBuoyancy.Reset();
	m_psFoamToLiquid.Reset();
	m_psFoamDecay.Reset();
	m_cbFoam.Release();
	m_foam.current = nullptr;
	m_foam.next = nullptr;

	m_psMassScale.Reset();
	m_massReadback.Reset();
	m_cbMassScale.Release();
	m_readbackValid = false;

	m_intensity.clear();
	m_connection = nullptr;

	m_quantity.current = nullptr;
	m_quantity.next = nullptr;
	m_displayTex = nullptr;

	m_initialized = false;
}

// ===================================================
// 初期 quantity データ（行優先。row0=上、row増加=下＝重力方向）
//  空のコップから開始（全セルを空気）。プレイヤーが上部から注いで貯める。
// ===================================================
void FluidField::BuildInitialQuantity(std::vector<Math::Vector4>& out) const
{
	const int w = WaterConst::kGridWidth;
	const int h = WaterConst::kGridHeight;

	out.resize(static_cast<size_t>(w) * h);

	for (int i = 0; i < w * h; ++i)
	{
		// (x,y=運動量=0, z=質量=空気, w=体積)
		out[i] = { 0.0f, 0.0f, WaterConst::kCellAirMass, WaterConst::kCellVolume };
	}
}

// ===================================================
// フルスクリーンパスを1回描画する（src を t0 に、ps で dst へ書き込む）
//  additive=true で純加算ブレンド（upsampleの圧力補正加算に使う）
// ===================================================
void FluidField::RenderPass(ID3D11PixelShader* ps,
	const std::shared_ptr<KdTexture>& src, const std::shared_ptr<KdTexture>& dst, bool additive)
{
	if (!dst) { return; }

	KdDirect3D& d3d = KdDirect3D::Instance();
	ID3D11DeviceContext* ctx = d3d.WorkDevContext();

	// dst をレンダーターゲットに設定（ビューポートも dst サイズに合わせる）
	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(dst->GetWidth());
	vp.Height = static_cast<float>(dst->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	KdRenderTargetChanger changer;
	changer.ChangeRenderTarget(dst, nullptr, &vp);

	// 加算（圧力補正）か上書き（不透明）か
	ctx->OMSetBlendState(additive ? m_additiveBlend.Get() : nullptr, nullptr, 0xFFFFFFFF);

	// パイプライン設定
	KdShaderManager& sm = KdShaderManager::Instance();
	sm.SetVertexShader(m_vsFullScreen.Get());
	sm.SetInputLayout(m_inputLayout.Get());
	sm.SetPixelShader(ps);

	// 入力テクスチャ（サンプラ不要：ピクセルシェーダは Load で取得）
	ID3D11ShaderResourceView* srv = src ? src->WorkSRView() : nullptr;
	ctx->PSSetShaderResources(0, 1, &srv);

	// フルスクリーン描画
	d3d.DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, m_screenVert, sizeof(ScreenVertex));

	// 入力解除（同じテクスチャを次パスで書き込み先にできるように）
	ID3D11ShaderResourceView* nullSrv = nullptr;
	ctx->PSSetShaderResources(0, 1, &nullSrv);
	if (additive) { ctx->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); }

	changer.UndoRenderTarget();
}

// ===================================================
// 2枚のテクスチャ(t0,t1)を入力にフルスクリーンパスを描画する（不透明上書き）
// ===================================================
void FluidField::RenderPass2(ID3D11PixelShader* ps,
	const std::shared_ptr<KdTexture>& src0, const std::shared_ptr<KdTexture>& src1,
	const std::shared_ptr<KdTexture>& dst)
{
	if (!dst) { return; }

	KdDirect3D& d3d = KdDirect3D::Instance();
	ID3D11DeviceContext* ctx = d3d.WorkDevContext();

	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(dst->GetWidth());
	vp.Height = static_cast<float>(dst->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	KdRenderTargetChanger changer;
	changer.ChangeRenderTarget(dst, nullptr, &vp);
	ctx->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	KdShaderManager& sm = KdShaderManager::Instance();
	sm.SetVertexShader(m_vsFullScreen.Get());
	sm.SetInputLayout(m_inputLayout.Get());
	sm.SetPixelShader(ps);

	ID3D11ShaderResourceView* srvs[2] =
	{
		src0 ? src0->WorkSRView() : nullptr,
		src1 ? src1->WorkSRView() : nullptr,
	};
	ctx->PSSetShaderResources(0, 2, srvs);

	d3d.DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, m_screenVert, sizeof(ScreenVertex));

	ID3D11ShaderResourceView* nullSrvs[2] = { nullptr, nullptr };
	ctx->PSSetShaderResources(0, 2, nullSrvs);

	changer.UndoRenderTarget();
}

// ===================================================
// 圧力解法：マルチグリッドV-cycle → 圧力勾配 → 加速 → 変位
//  みずあそび.html の update() 圧力ブロックを移植。
// ===================================================
void FluidField::SolvePressure()
{
	if (m_intensity.empty()) { return; }

	const int levels = static_cast<int>(m_intensity.size());

	// ① intensityピラミッド最上段を quantity から構築
	RenderPass(m_psCompression.Get(), m_quantity.current, m_intensity[0].current);

	// ② 下り：各レベルでヤコビ反復 → 1つ粗いレベルへ downsample
	for (int level = 0; level < levels; ++level)
	{
		for (int it = 0; it < WaterConst::kJacobiIterations; ++it)
		{
			RenderPass(m_psSolution.Get(), m_intensity[level].current, m_intensity[level].next);
			m_intensity[level].Swap();
		}
		if (level + 1 < levels)
		{
			RenderPass(m_psDownsample.Get(), m_intensity[level].current, m_intensity[level + 1].current);
		}
	}

	// ③ 上り：各レベルでヤコビ反復 → 1つ細いレベルへ upsample（加算補正）
	for (int level = levels - 1; level >= 0; --level)
	{
		for (int it = 0; it < WaterConst::kJacobiIterations; ++it)
		{
			RenderPass(m_psSolution.Get(), m_intensity[level].current, m_intensity[level].next);
			m_intensity[level].Swap();
		}
		if (level - 1 >= 0)
		{
			RenderPass(m_psUpsample.Get(), m_intensity[level].current, m_intensity[level - 1].current, true);
		}
	}

	// ④ 圧力勾配 → connection
	RenderPass2(m_psPressureGradient.Get(), m_quantity.current, m_intensity[0].current, m_connection);

	// ⑤ 圧力で加速（運動量へ）
	RenderPass2(m_psPressureAcceleration.Get(), m_quantity.current, m_connection, m_quantity.next);
	m_quantity.Swap();

	// ⑥ 質量再配置（非圧縮化）
	RenderPass2(m_psPressureDisplacement.Get(), m_quantity.current, m_connection, m_quantity.next);
	m_quantity.Swap();
}

// ===================================================
// 炭酸：泡を1回進める
//  発生（液→泡）→ 浮上＋拡散 → 減衰（泡→液で水位上昇）
// ===================================================
void FluidField::SolveFoam()
{
	if (!m_psFoamGen) { return; }

	m_cbFoam.Write();
	KdShaderManager::Instance().SetPSConstantBuffer(0, m_cbFoam.GetAddress());

	// ① 発生：乱流から泡を作り（foam += gen）、同量を液の質量から引く（z -= gen）
	RenderPass2(m_psFoamGen.Get(), m_quantity.current, m_foam.current, m_foam.next);
	m_foam.Swap();
	RenderPass(m_psFoamLoss.Get(), m_quantity.current, m_quantity.next);
	m_quantity.Swap();

	// ② 浮上＋水平拡散（液面に泡の頭を作る）
	RenderPass2(m_psFoamBuoyancy.Get(), m_foam.current, m_quantity.current, m_foam.next);
	m_foam.Swap();

	// ③ 減衰ぶんを液へ戻し（水位上昇）、泡を減衰させる
	RenderPass2(m_psFoamToLiquid.Get(), m_quantity.current, m_foam.current, m_quantity.next);
	m_quantity.Swap();
	RenderPass(m_psFoamDecay.Get(), m_foam.current, m_foam.next);
	m_foam.Swap();
}

// ===================================================
// 移流パス：1セル1点を GS で小クアッドへ展開し、純加算で dst へ撒き込む
// ===================================================
void FluidField::RenderAdvection(const std::shared_ptr<KdTexture>& src, const std::shared_ptr<KdTexture>& dst)
{
	if (!dst || !src) { return; }

	KdDirect3D& d3d = KdDirect3D::Instance();
	ID3D11DeviceContext* ctx = d3d.WorkDevContext();
	KdShaderManager& sm = KdShaderManager::Instance();

	// dst をレンダーターゲットに設定
	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(dst->GetWidth());
	vp.Height = static_cast<float>(dst->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	KdRenderTargetChanger changer;
	changer.ChangeRenderTarget(dst, nullptr, &vp);

	// 撒き込み先を 0 でクリア（加算合成の土台）
	const float clearZero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ctx->ClearRenderTargetView(dst->WorkRTView(), clearZero);

	// シェーダ設定（VS→GS→PS）
	m_cbAdvection.Write();
	sm.SetVertexShader(m_vsAdvection.Get());
	sm.SetInputLayout(m_advectionLayout.Get());
	sm.SetPixelShader(m_psAdvection.Get());
	sm.SetVSConstantBuffer(0, m_cbAdvection.GetAddress());
	ctx->GSSetShader(m_gsAdvection.Get(), nullptr, 0);
	ctx->GSSetConstantBuffers(0, 1, m_cbAdvection.GetAddress());

	// 入力：現在の物理量（頂点シェーダで Load する）
	ID3D11ShaderResourceView* srv = src->WorkSRView();
	ctx->VSSetShaderResources(0, 1, &srv);

	// 純加算ブレンド
	ctx->OMSetBlendState(m_additiveBlend.Get(), nullptr, 0xFFFFFFFF);

	// 1セル1点を POINTLIST で描画
	UINT stride = sizeof(CellVertex);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, m_cellPosVB.GetAddressOf(), &stride, &offset);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	ctx->Draw(static_cast<UINT>(WaterConst::kGridWidth) * WaterConst::kGridHeight, 0);

	// 後始末：GS解除・入力解除・ブレンド戻し
	ctx->GSSetShader(nullptr, nullptr, 0);
	ID3D11ShaderResourceView* nullSrv = nullptr;
	ctx->VSSetShaderResources(0, 1, &nullSrv);
	ctx->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	changer.UndoRenderTarget();
}
