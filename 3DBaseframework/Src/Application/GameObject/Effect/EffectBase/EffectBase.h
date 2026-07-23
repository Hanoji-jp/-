#pragma once

class EffectBase : public KdGameObject
{
public:

	EffectBase() { Init(); }
	~EffectBase() {}

	virtual void Init()							override;
	virtual void Update()						override {}
	virtual void DrawLit()						override {}
	virtual void DrawSprite()					override {}

	// アクティブ化　成功（良）時だけ表示する。初期は無効。
	virtual void Activate() { m_activeFlg = true; }

	// 非アクティブ化
	virtual void Deactivate() { m_activeFlg = false; }

	// アクティブ状態確認関数
	virtual bool IsActiveFlg() const { return m_activeFlg; }

	// 終了確認関数
	virtual bool IsEndFlg() const { return m_endFlg; }

	// 拡縮関数
	virtual void ScalingIteration(float _scaleMax, float _scaleMin, float _speed);

	// キャラ配置演出（拡大→縮小のワンショット）
	virtual void PlaceChara();

protected:
	//====================
	// 実行用フラグなど
	//====================
	// アクティブ状態フラグ
	bool m_activeFlg = false;

	// エフェクト終了フラグ
	bool m_endFlg = false;

	//====================
	// 描画関係
	//====================
	// 板ポリゴン
	std::shared_ptr<KdSquarePolygon>	m_spPolygon = nullptr;

	// 2D画像
	std::shared_ptr<KdTexture> m_spTex = nullptr;

	// 行列 (拡大、回転、移動)
	Math::Matrix m_mScale = Math::Matrix::Identity;
	Math::Matrix m_mRotate = Math::Matrix::Identity;
	Math::Matrix m_mTrans = Math::Matrix::Identity;

	// 画像サイズ
	Math::Vector2 m_texSize = { 0.0f,0.0f };

	// 拡縮
	float	m_scale		= 1.0f;

	// 拡縮反復の向き（true=拡大中/ false=縮小中）。
	//  ※以前は関数ローカル static で全インスタンス共有かつ永続だったため、
	//    前の状態が残って拡縮が止まって見えることがあった。インスタンスごとに持つ。
	bool	m_scaleUp	= true;

	// PlaceChara（ポップイン演出）のタイマー。0で停止。Activateで kPlaceDuration へリセットして再生。
	//  ※これも以前は関数ローカル static で、2回目以降アニメせず・スケールが負になる不具合があった。
	static constexpr int kPlaceDuration = 20;	// アニメ長（フレーム）
	int		m_placeTime	= 0;

	//====================
	// 移動関係
	//====================
	// 座標
	Math::Vector3 m_pos = { 0.0f,0.0f,0.0f };

	// 移動量
	Math::Vector3 m_move = { 0.0f,0.0f,0.0f };

	// 移動速度
	float m_speed = 0.05f;
};
