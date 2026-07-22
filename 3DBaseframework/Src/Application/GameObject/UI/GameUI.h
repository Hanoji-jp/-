#pragma once

#include "UIConst.h"

// 前方宣言
class Water;

// ===================================================
// ゲームUI（判定演出）
//  水量と目標ラインを比較し、「良！」などの結果を表示する
// ===================================================
class GameUI : public KdGameObject
{
public:
	GameUI()					{}
	~GameUI()		override	{}

	void Init()		override;
	void Update()	override;
	void DrawSprite() override;

	// 判定対象の水をセット
	void SetTargetWater(const std::shared_ptr<Water>& water) { m_wpWater = water; }

	// 現在の判定結果
	UIConst::RyoResult GetResult() const { return m_result; }

private:
	// 判定対象の水
	std::weak_ptr<Water> m_wpWater;

	// 現在の判定結果
	UIConst::RyoResult m_result = UIConst::RyoResult::None;
};
