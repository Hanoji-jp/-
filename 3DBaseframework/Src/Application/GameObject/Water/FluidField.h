#pragma once

// 前方宣言
class KdTexture;

// ===================================================
// コップ内2次元自由表面流体（GPUマルチパス）
//  みずあそび.html（catincat.jp）の方式を DirectX11 へ移植する流体場。
//  quantity テクセル = (x,y=運動量, z=質量, w=体積)。
//
//  Phase1（基盤）:
//   ・float レンダーターゲットの ping-pong
//   ・フルスクリーンパスの描画（VS/PS のロードと実行）
//   ・可視化テクスチャの生成（コップへ表示する用）
//  以降のフェーズで、移流・圧力解法・注水・本描画のパスを積み上げる。
// ===================================================
class FluidField
{
public:
	FluidField() {}
	~FluidField() { Release(); }

	// リソース・シェーダ生成、初期状態の投入
	bool Init();

	// シミュレーションを1ステップ進める（GPUパスを回す）
	//  レンダーターゲットは内部で切り替え、呼び出し前の状態へ復元する。
	void Step(float deltaTime);

	// 初期状態へ戻す
	void Reset();

	// 解放
	void Release();

	// コップへ表示する可視化テクスチャ（現在の水の状態）
	const std::shared_ptr<KdTexture>& GetDisplayTexture() const { return m_displayTex; }

private:
	// quantity（xy=運動量, z=質量, w=体積）の ping-pong バッファ
	struct DoubleBuffer
	{
		std::shared_ptr<KdTexture> current = nullptr;	// 読み取り側
		std::shared_ptr<KdTexture> next    = nullptr;	// 書き込み側
		void Swap() { std::swap(current, next); }
	};

	// 全セルの初期 quantity データ（行優先。row0=上、row増加=下＝重力方向）を作る
	void BuildInitialQuantity(std::vector<Math::Vector4>& out) const;

	// フルスクリーンパスを1回描画する（src を入力に ps で dst へ書き込む）
	void RenderPass(ID3D11PixelShader* ps,
		const std::shared_ptr<KdTexture>& src, const std::shared_ptr<KdTexture>& dst);

	// 可視化パスの色定数バッファ
	struct cbVisualize
	{
		Math::Vector4 WaterColor;	// 水の色
		Math::Vector4 SpaceColor;	// 空(水なし)の色
	};

	// フルスクリーン描画用の頂点（クリップ空間）
	struct ScreenVertex
	{
		Math::Vector3 Pos;
	};

	// -------- リソース --------
	DoubleBuffer				m_quantity;						// 物理量バッファ
	std::shared_ptr<KdTexture>	m_displayTex = nullptr;			// 表示用（RGBA8）

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vsFullScreen;	// フルスクリーンVS
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psCopy;		// 恒等コピーPS
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psVisualize;	// 可視化PS

	KdConstantBuffer<cbVisualize>	m_cbVisualize;				// 可視化色

	ScreenVertex	m_screenVert[4] = {};						// フルスクリーン4頂点

	bool			m_initialized = false;
};
