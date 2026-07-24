#pragma once

// ===================================================
// 水（簡易2Dグリッド流体：1次元高さ場／浅水近似）関連の定数
// ===================================================
namespace WaterConst
{
	// デバッグ表示用：直近の実測水位(0..1)。Water::PreDrawが毎フレーム書き込み、ImGuiが表示する。
	//  見た目の水面と判定値のズレを確認・調整するために使う。
	inline float g_debugFillRate = 0.0f;

	// 水面を左右に分割する列（グリッド）の数
	inline constexpr int kColumnCount = 60;

	// 波の伝播の強さ（隣の列へ高さ差が伝わる強さ）
	inline constexpr float kWaveSpread = 8.0f;

	// 速度の減衰（1秒あたり）：波を徐々に収める
	inline constexpr float kVelocityDamp = 1.2f;

	// シミュレーションに使う最大dt（大きすぎると発散するので上限を設ける）
	inline constexpr float kMaxDeltaTime = 1.0f / 30.0f;

	// 注水：着水点の水面が上がる速さ（メートル/秒）
	inline constexpr float kPourRisePerSec = 2.5f;

	// 注水の広がり（着水点から左右に何列ぶんに水を足すか）
	inline constexpr int kPourHalfWidth = 3;

	// 開始時に入れておく水位（0.0～1.0）：描画確認しやすいよう少し入れておく
	inline constexpr float kInitialFillRate = 0.3f;

	// 水の色（上面・底で濃淡をつける）
	inline const Math::Color kColorTop    = { 0.35f, 0.70f, 0.98f, 1.0f };
	inline const Math::Color kColorBottom = { 0.13f, 0.42f, 0.82f, 1.0f };

	// 自己発光の強さ（陰影で暗くなりすぎず水色が見えるように）
	inline constexpr float kEmissiveRate = 0.5f;

	// ===============================================
	// 2次元グリッド流体（GPUマルチパス）用の定数
	//  コップ内側の矩形領域に対応させたグリッドで水を解く。
	//  quantity テクセル = (x,y=運動量, z=質量, w=体積)
	// ===============================================

	// 流体グリッドの解像度＝水の「粒（1セル）」の細かさ。上げるほど粒が小さく（細かく）なる。
	//  領域は「コップ内側(幅4.0×高さ5.0)＋上のシュート(高さ5.0)」＝幅4.0×高さ10.0（比 4:10）。
	//  セルを正方形に保つため 高さ行数=幅列数×(10.0/4.0)=幅×2.5。
	//  ImGui(Tuning)で変更→Apply で流体を作り直して反映（mutable。上げ過ぎると重い）。
	inline int kGridWidth  = 160;	// 粒の細かさ（列数）。基準128の1.25倍
	inline int kGridHeight = 400;	// = kGridWidth * 2.5（正方セル維持）

	// ImGuiで解像度を変えたとき、流体場を作り直す要求フラグ（Water::PreDrawが処理）
	inline bool g_fluidReinitRequested = false;

	// セル初期値：質量／体積
	inline constexpr float kCellWaterMass = 1.0f;	// 水で満たされたセルの質量
	inline constexpr float kCellAirMass   = 0.001f;	// 水の無いセルの質量（完全な0は避ける）
	inline constexpr float kCellVolume    = 1.0f;	// セルの体積（全セル共通）

	// 水の3D回転体（表示）の色・形
	inline const Math::Color kWaterBodyColor = { 0.20f, 0.55f, 0.90f, 0.80f };	// 半透明の青
	inline constexpr float   kWaterInset     = 0.05f;	// ガラス内壁より少し内側に収める量

	// 水位測定：この質量を超えるセルを「水」とみなし、各列の最上水セルから水面高さを測る。
	//  判定（良/Under/Over）は質量割合ではなく、この実測の水面高さで行う
	//  （多角形の台形グラスでは質量割合と水面高さが一致しないため）。
	//  ★重要：可視化(kVisualizeWaterLo/Hi=0.18/0.34)の中点(≈0.26)に合わせる。
	//   大きい(0.5)と、見た目より低く測って「見た目はライン=なのに Under」＝良が出ない不整合になる。
	inline constexpr float kLevelMassThresh = 0.26f;

	// 水面の見た目：実測レベルの水平面に「ゆるやかな波」を足して無機質さを消す。
	//  ・壁際の盛り上がりは出さず、xの正弦波で上下に軽く揺らすだけ。
	//  ※ImGuiで実行時に変更できるよう mutable（inline 非const）にしている。
	inline float kSurfaceWaveAmp   = 0.022f;	// 波の振幅（高さに対する割合。0で完全な平面）
	inline constexpr float kSurfaceWaveSpeed = 0.08f;	// 1ステップあたりの波の進み（位相）
	// 注ぐ水（落下ストリーム）を見せる中央帯の半幅（幅に対する割合）。ここだけ水面より上のシム水を描く。
	inline float kStreamHalfFrac   = 0.12f;

	// 可視化パスの色：空色⇔水色を質量で補間、その上に泡色を重ねる
	inline const Math::Color kVisualizeSpaceColor = { 0.87f, 0.93f, 0.98f, 1.0f };	// 空き（淡い水色）
	//  水の基準色（rgb）と基準アルファ(a)。ImGuiで実行時に変更できるよう mutable にしている。
	inline Math::Color kVisualizeWaterColor = { 0.20f, 0.55f, 0.90f, 0.78f };	// 水（青・半透明）
	inline const Math::Color kVisualizeFoamColor  = { 0.97f, 0.99f, 1.00f, 1.0f };	// 泡（白）
	inline constexpr float   kVisualizeFoamGain   = 2.5f;	// 泡の見え方（泡量→不透明度）

	// 水の塗り分け閾値：質量(z)がこの範囲で 空色→水色 へ切り替える。
	//  Lo を上げ Hi を下げるほど、薄いセルも水として塗られ「気泡（粒々）」が減る。
	inline constexpr float kVisualizeWaterLo = 0.18f;	// これ以下は空気
	inline constexpr float kVisualizeWaterHi = 0.34f;	// これ以上は完全に水

	// ===============================================
	// Phase2：移流＋重力
	// ===============================================

	// 重力（1ステップあたりの落下量：グリッドセル単位。row増加＝画面下が +）＝落下（注ぐ）の速さ。
	//  大きいほど水が速く落ちる。小さいほどゆっくり注がれる。ImGui(Tuning)で実行時に調整するため mutable。
	//  ※上げ過ぎると1ステップの移動が大きく移流で質量がこぼれる（水が減る）。安定域に抑える。
	inline float kGravityPerStep = 0.14f;	// 落下スピード（既定を下げてゆっくり注ぐ）

	// 運動量の減衰（移流で速度に掛ける。1未満で放置時のスロッシュ＝壁際の盛り上がりが収まる）
	//  ※重力には掛けない（落下は保つ）。1.0で減衰なし。1に近いほど水が柔らかく・良く動く。
	inline float kVelocityDampPerStep = 0.965f;

	// 初期の水ブロック（グリッドに対する割合。row0=上）
	//  上部中央に塊を置き、重力で落下して底に溜まる様子を確認する
	inline constexpr float kInitBlockLeftRate   = 0.25f;	// 左端
	inline constexpr float kInitBlockRightRate  = 0.75f;	// 右端
	inline constexpr float kInitBlockTopRate    = 0.08f;	// 上端（小さいほど上）
	inline constexpr float kInitBlockBottomRate = 0.42f;	// 下端

	// ===============================================
	// Phase3：圧力（マルチグリッド解法）
	// ===============================================

	// 各レベルでの圧力ヤコビ反復回数（多いほど水面が平らに落ち着く＝左右の盛り上がりが減る）
	inline constexpr int kJacobiIterations = 12;

	// intensityピラミッドを作る最小サイズ（幅・高さがこれ未満になったら止める）
	inline constexpr int kMinPyramidSize = 8;

	// ===============================================
	// Phase4：注水（pour）と補填（equalization）
	// ===============================================

	// 吸い込み口（上部）の形（グリッドのセル単位）※解像度2倍に合わせてセル数も2倍
	inline constexpr int kPourHalfCols = 16;	// 中心から左右へ何列ぶんか（幅 = 2*これ）。シュートのチャネル幅に合わせる
	inline constexpr int kPourTopRow   = 1;	// 上端から何row下に置くか（小さいほど水がコップの一番上から湧く＝口に近い）
	inline constexpr int kPourRows     = 6;	// 縦方向の厚み（row数）
	// 注ぎ口の横位置オフセット（列。中央=0。＋で右へ）＝ピッチャーの口の下から水を出すため
	inline constexpr int kPourCenterColOffset = 0;

	// 手を離してから判定を確定するまでの沈静フレーム数（60fpsで約2.5秒）。
	//  水が跳ねて暴れている最中に判定すると水面が読めず、良（クリア）が正しく出ない。
	//  シュートの蓋(kChuteSealDelayFrames=90)より後に判定するよう、それより大きくする。
	inline constexpr int kJudgeSettleFrames = 150;

	// 止水してからシュート（落水路）を蓋で塞ぐまでの遅延フレーム数。
	//  落下途中の水柱がコップに着き切ってから塞ぐことで、水位の急落を防ぐ（60fpsで約1.5秒）。
	//  塞ぐと、溜まった水が上へ這い上がって減っていくのを止められる。
	inline constexpr int kChuteSealDelayFrames = 90;

	// 1ステップで注ぐ量・下向き速度（ImGui(Tuning)で実行時に調整するため mutable）。
	inline float kPourMassPerStep = 0.5f;	// セルあたりに加える質量（大きいほど速く・多く入る）＝注ぐスピード
	inline float kPourVelocity    = 0.38f;	// 加える下向き速度（大きいほど注ぎがきびきび速い。上げ過ぎると水が減る）

	// 反拡散（anti-diffusion）パス（旧equalizationの配管を流用）。
	//  移流のバイリニア撒き込みで毎フレーム鈍る水/空気の界面を、質量zの負のラプラシアンで
	//  鋭く戻す（保存的）。これで止水後も水面が下がらない（シムは回したまま拡散だけ打ち消す）。
	inline constexpr bool  kEnableEqualization = true;
	//  反拡散の強さ（0で無効、大きいほど強く再集中。上げ過ぎると不安定＝振動/発散。安定域は~0.03〜0.15）。
	inline float kAntiDiffusionRate = 0.0f;	// 既定0＝無効（反拡散は過剰集中で暴走するため。実験用にスライダーは残す）
	// 旧equalizationの未使用パラメータ（CB構造体の互換のため残置）
	inline constexpr float kEqualizationSurplusRate = 0.0f;
	inline constexpr float kEqualizationDeficitRate = 0.0f;
	inline constexpr float kEqualizationRefillDensity = 1.0f;

	// ===============================================
	// 炭酸水：泡（foam）
	//  注水の乱流から泡が発生し、液の質量を預かって浮上→液面に溜まり、
	//  減衰して液へ戻る（＝泡が消えると水位が上がる）。総質量は保存。
	//  ※ kEnableCarbonation=false で泡パスをスキップし「普通の水」になる。
	// ===============================================

	// 炭酸（泡）を有効にするか。false で普通の水。
	inline constexpr bool kEnableCarbonation = false;

	inline constexpr float kFoamGenRate      = 0.04f;	// 乱流1あたり 液→泡へ移す量
	inline constexpr float kFoamSpeedThresh  = 0.35f;	// これ以上の速さで泡が立つ
	inline constexpr float kFoamMassThresh   = 0.30f;	// 液とみなす最小質量（浮上・液面判定に使う）
	inline constexpr float kFoamRiseRate     = 0.60f;	// 浮上の速さ（1ステップの移動割合）
	inline constexpr float kFoamSpreadRate   = 0.12f;	// 泡の水平拡散（頭を平らに）
	inline constexpr float kFoamDecayRate    = 0.96f;	// 1ステップで残る泡の割合（残り=液へ戻る）
}
