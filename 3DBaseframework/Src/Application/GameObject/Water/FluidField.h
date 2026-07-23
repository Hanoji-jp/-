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
	//  pouring=true の間は上部中央から注水する。
	void Step(float deltaTime, bool pouring);

	// 初期状態へ戻す
	void Reset();

	// 解放
	void Release();

	// コップへ表示する可視化テクスチャ（現在の水の状態）
	const std::shared_ptr<KdTexture>& GetDisplayTexture() const { return m_displayTex; }

	// 現在の水位（0.0=空 ～ 1.0=満杯）。注いだ水量から算出（読み戻し不要）
	float GetFillRate() const;

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

	// フルスクリーンパスを1回描画する（src を t0 に、ps で dst へ書き込む）
	//  additive=true で純加算ブレンド（upsampleの圧力補正加算に使う）
	void RenderPass(ID3D11PixelShader* ps,
		const std::shared_ptr<KdTexture>& src, const std::shared_ptr<KdTexture>& dst, bool additive = false);

	// 2枚のテクスチャ(t0,t1)を入力にフルスクリーンパスを描画する
	void RenderPass2(ID3D11PixelShader* ps,
		const std::shared_ptr<KdTexture>& src0, const std::shared_ptr<KdTexture>& src1,
		const std::shared_ptr<KdTexture>& dst);

	// 移流パス：src を入力に、1セル1点を GS で撒き込んで dst へ加算合成する
	void RenderAdvection(const std::shared_ptr<KdTexture>& src, const std::shared_ptr<KdTexture>& dst);

	// 圧力解法（マルチグリッドV-cycle＋圧力の加速・変位）を1回実行する
	void SolvePressure();

	// 泡（炭酸）を1回進める（発生・浮上・拡散・減衰＝液へ還元）
	void SolveFoam();

	// 可視化パスの色定数バッファ
	struct cbVisualize
	{
		Math::Vector4 WaterColor;	// 水の色
		Math::Vector4 SpaceColor;	// 空(水なし)の色
		Math::Vector4 FoamColor;	// 泡の色
		Math::Vector4 FoamParams;	// x=泡の見え方ゲイン
	};

	// 泡（炭酸）パスの定数バッファ
	struct cbFoam
	{
		float GenRate = 0.0f;		// 乱流→泡 の量
		float SpeedThresh = 0.0f;	// 泡が立つ速さのしきい
		float MassThresh = 0.0f;	// 液とみなす最小質量
		float RiseRate = 0.0f;		// 浮上の速さ
		float SpreadRate = 0.0f;	// 水平拡散
		float DecayRate = 0.0f;		// 残る割合（残り=液へ戻る）
		Math::Vector2 Pad;
	};

	// 移流パスの定数バッファ
	struct cbAdvection
	{
		Math::Vector2 Gravity;		// 重力（row増加方向＝画面下が +）
		Math::Vector2 GridSize;		// グリッド解像度 (W, H)
	};

	// 移流用の頂点（1セル1点：セル中心のピクセル座標）
	struct CellVertex
	{
		Math::Vector2 Pos;
	};

	// 注水パスの定数バッファ
	struct cbPour
	{
		Math::Vector4 InletRect;	// 吸い込み口 (colMin, rowMin, colMax, rowMax)
		float MassRate = 0.0f;		// 1ステップで加える質量（セルあたり）
		float Velocity = 0.0f;		// 加える下向き速度
		Math::Vector2 Pad;
	};

	// 補填パスの定数バッファ
	struct cbEqualization
	{
		float SurplusRate = 0.0f;	// 余剰を削る率
		float DeficitRate = 0.0f;	// 不足を補う率
		Math::Vector2 Pad;
		Math::Vector4 DeficitRatio;	// 補填で加える比率 (0,0, mass/volume, 1)
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
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;	// フルスクリーン入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psCopy;		// 恒等コピーPS
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psVisualize;	// 可視化PS
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psSurface;	// 表面（体積フラックス）PS

	// 移流（scatter）用
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vsAdvection;		// 移流VS
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_gsAdvection;		// 移流GS（点→クアッド）
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_psAdvection;		// 移流PS（重み付き撒き込み）
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_advectionLayout;	// 移流入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cellPosVB;		// 1セル1頂点の位置バッファ
	Microsoft::WRL::ComPtr<ID3D11BlendState>		m_additiveBlend;	// 純加算ブレンド(ONE,ONE)

	// 圧力解法（マルチグリッド）用
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psCompression;			// intensity最上段の構築
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psDownsample;				// 細→粗
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psUpsample;				// 粗→細（加算）
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psSolution;				// 圧力ヤコビ反復
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psPressureGradient;		// 圧力勾配→connection
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psPressureAcceleration;	// 圧力で加速
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psPressureDisplacement;	// 質量再配置

	std::vector<DoubleBuffer>	m_intensity;					// intensityピラミッド（W×H→…→8以上）
	std::shared_ptr<KdTexture>	m_connection = nullptr;			// 圧力勾配（4近傍への力）

	// 注水・補填
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psPour;			// 注水
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psEqualization;	// 補填
	KdConstantBuffer<cbPour>			m_cbPour;					// 吸い込み口・注水量
	KdConstantBuffer<cbEqualization>	m_cbEqualization;			// 補填パラメータ

	int		m_inletCellCount = 0;	// 吸い込み口のセル数（注水量の積算に使う）
	float	m_waterMass = 0.0f;		// 現在の水量（質量の合計。CPUで積算）

	// 炭酸：泡
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psFoamGen;		// 泡の発生
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psFoamLoss;		// 発生ぶんの液質量減算
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psFoamBuoyancy;	// 浮上＋拡散
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psFoamToLiquid;	// 泡→液へ戻す
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_psFoamDecay;		// 泡の減衰
	KdConstantBuffer<cbFoam>	m_cbFoam;							// 泡パラメータ
	DoubleBuffer				m_foam;								// 泡フィールド（.r=泡量）

	KdConstantBuffer<cbVisualize>	m_cbVisualize;				// 可視化色
	KdConstantBuffer<cbAdvection>	m_cbAdvection;				// 重力・グリッドサイズ

	ScreenVertex	m_screenVert[4] = {};						// フルスクリーン4頂点

	bool			m_initialized = false;
};
