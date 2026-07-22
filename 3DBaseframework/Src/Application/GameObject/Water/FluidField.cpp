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

	// -------- 定数バッファ（可視化色） --------
	{
		cbVisualize init;
		const Math::Color& w = WaterConst::kVisualizeWaterColor;
		const Math::Color& s = WaterConst::kVisualizeSpaceColor;
		init.WaterColor = { w.R(), w.G(), w.B(), w.A() };
		init.SpaceColor = { s.R(), s.G(), s.B(), s.A() };
		m_cbVisualize.Create(&init);
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

	m_initialized = true;
	return true;
}

// ===================================================
// 1ステップ進める
//  Phase1：恒等コピー(ping-pong疎通) → 可視化テクスチャ生成
// ===================================================
void FluidField::Step(float deltaTime)
{
	if (!m_initialized) { return; }
	(void)deltaTime;	// Phase1では時間刻みは未使用（以降のフェーズで移流・重力へ使う）

	// 物理量をそのまま次バッファへ写す（float RT の読み書きと ping-pong の確認）
	RenderPass(m_psCopy.Get(), m_quantity.current, m_quantity.next);
	m_quantity.Swap();

	// 現在の物理量から表示用テクスチャを生成
	m_cbVisualize.Write();
	KdShaderManager::Instance().SetPSConstantBuffer(0, m_cbVisualize.GetAddress());
	RenderPass(m_psVisualize.Get(), m_quantity.current, m_displayTex);
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
}

// ===================================================
// 解放
// ===================================================
void FluidField::Release()
{
	m_cbVisualize.Release();

	m_vsFullScreen.Reset();
	m_inputLayout.Reset();
	m_psCopy.Reset();
	m_psVisualize.Reset();

	m_quantity.current = nullptr;
	m_quantity.next = nullptr;
	m_displayTex = nullptr;

	m_initialized = false;
}

// ===================================================
// 初期 quantity データ（行優先。row0=上、row増加=下＝重力方向）
//  コップ下部 kInitialFillRate ぶんを水で満たす
// ===================================================
void FluidField::BuildInitialQuantity(std::vector<Math::Vector4>& out) const
{
	const int w = WaterConst::kGridWidth;
	const int h = WaterConst::kGridHeight;

	out.resize(static_cast<size_t>(w) * h);

	// 上から数えて、この行より下(=大きいrow)は水で満たす
	const int waterTopRow = static_cast<int>(h * (1.0f - WaterConst::kInitialFillRate));

	for (int row = 0; row < h; ++row)
	{
		const bool isWater = (row >= waterTopRow);
		const float mass = isWater ? WaterConst::kCellWaterMass : WaterConst::kCellAirMass;

		for (int col = 0; col < w; ++col)
		{
			// (x,y=運動量=0, z=質量, w=体積)
			out[static_cast<size_t>(row) * w + col] = { 0.0f, 0.0f, mass, WaterConst::kCellVolume };
		}
	}
}

// ===================================================
// フルスクリーンパスを1回描画する（src を入力に ps で dst へ書き込む）
// ===================================================
void FluidField::RenderPass(ID3D11PixelShader* ps,
	const std::shared_ptr<KdTexture>& src, const std::shared_ptr<KdTexture>& dst)
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

	changer.UndoRenderTarget();
}
