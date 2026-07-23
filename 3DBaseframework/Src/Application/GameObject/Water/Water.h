#pragma once

#include "../UI/UIConst.h"	// RyoResult（判定結果）

// 前方宣言
class FluidField;

// ===================================================
// 水面表示用の板ポリゴン
//  コップ内側の矩形に、流体の可視化テクスチャを貼るための4頂点を持つ。
// ===================================================
class WaterDisplayPolygon : public KdPolygon
{
public:
	WaterDisplayPolygon() { Set2DObject(false); }	// 3D空間の板ポリとして扱う

	// 頂点を差し替える（基底の protected 頂点配列へ書き込む）
	void SetVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }
};

// ===================================================
// 水（コップ内2次元自由表面流体：GPUマルチパス）
//  みずあそび.html の方式を移植した FluidField を駆動し、
//  コップ内側の板ポリゴンへ水面を表示する。
//
// 
//   ・FluidField による float RT の ping-pong ／ フルスクリーンパス
//   ・可視化テクスチャをコップ内側へ表示
//  以降のフェーズで、移流・圧力解法・注水・本描画を積み上げる。
// ===================================================
class Water : public KdGameObject
{
public:
	Water();
	~Water()		override;	// FluidField を完全型で破棄するため実装は .cpp 側

	void Init()		override;
	void PreDraw()	override;	// GPUシミュレーションを1ステップ進める
	void Update()	override;
	void DrawUnLit()override;	// コップ内側へ水面を表示

	// 注水の状態（一発勝負：離したら終了してロック）
	enum class PourState
	{
		Ready,		// 未注水
		Pouring,	// 注水中
		Done,		// 終了（もう注げない）
	};

	// 注いでいるか（SPACEの押下状態）を毎フレーム設定する。
	//  一発勝負：一度離すと Done になり、以後は注水しない。
	void SetPouring(bool pouring);

	// 現在の水位（0.0=空 ～ 1.0=満杯）。FluidFieldが注水量から算出する
	float GetFillRate() const;

	// 注水が終了したか（判定確定）
	bool IsPourFinished() const { return m_pourState == PourState::Done; }

	// 判定結果（良／多い／少ない）
	UIConst::RyoResult GetResult() const { return m_result; }

	// リセット（空に戻し、もう一度注げるようにする）
	void Reset();

private:
	// 注水終了時に水位と目標ラインを比較して結果を確定する
	void JudgeResult();

	// 2次元流体場（GPU）。前方宣言＋スマートポインタで保持
	std::unique_ptr<FluidField> m_fluid;

	// コップ内側へ水面を貼る表示用板ポリゴン
	WaterDisplayPolygon m_displayPoly;

	// 注水の状態
	PourState m_pourState = PourState::Ready;

	// 判定結果
	UIConst::RyoResult m_result = UIConst::RyoResult::None;
};
