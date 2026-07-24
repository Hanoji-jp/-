#include "Tension.h"
#include "TensionConst.h"
#include "../Camera/CameraBase.h"
#include "../Water/Water.h"
#include "../Cup/CupConst.h"
#include "../GameStart/GameStart.h"

void Tension::Init()
{
	m_blackTex.Load(TensionConst::kBlackImage);
	// 心臓音は開始演出（3・2・1・start）が終わってから鳴らす（Update で開始）。
	m_started   = false;
	m_intensity = 0.0f;
}

void Tension::Update()
{
	// 開始演出が終わるまでは何もしない（start してから心臓音を鳴らす）。
	if (!m_started)
	{
		bool introDone = true;	// 開始演出が無ければ即開始
		if (auto gs = m_wpGameStart.lock()) { introDone = gs->IsFinished(); }
		if (!introDone) { return; }

		// start：心臓音を鳴らし始める（籠り・小音量から）。
		m_heart   = KdAudioManager::Instance().Play(TensionConst::kHeartAudio,   true);	// 普通
		m_muffled = KdAudioManager::Instance().Play(TensionConst::kMuffledAudio, true);	// 籠り
		if (m_muffled) { m_muffled->SetVolume(TensionConst::kHeartVolMin); }
		if (m_heart)   { m_heart->SetVolume(0.0f); }
		m_started = true;
	}

	// 目標ラインへの近さ（0=遠い〜1=目標付近）を演出の強さにする。
	float fill = 0.0f;
	if (auto w = m_wpWater.lock()) { fill = w->GetFillRate(); }
	const float target = std::clamp(fill / CupConst::kTargetLineRate, 0.0f, 1.0f);

	// 「少しずつ」変化させるため滑らかに追従
	m_intensity += (target - m_intensity) * TensionConst::kLerpRate;

	// 心臓音：全体音量は小→大（fillが溜まるほど大きく）。音色は籠り→普通へクロスフェード。
	const float gain = TensionConst::kHeartVolMin
		+ (TensionConst::kHeartVolMax - TensionConst::kHeartVolMin) * m_intensity;
	if (m_muffled) { m_muffled->SetVolume(gain * (1.0f - m_intensity)); }	// 籠り：最初が主、徐々に消える
	if (m_heart)   { m_heart->SetVolume(gain * m_intensity); }				// 普通：溜まるほど主に

	// カメラをコップへズームイン（intensity ぶんだけ近づける）
	if (auto cam = m_wpCamera.lock())
	{
		const float z = -(CupConst::kCameraDistanceZ - m_intensity * TensionConst::kZoomAmount);
		cam->SetPos({ CupConst::kCenterX, 0.0f, z });
	}
}

void Tension::DrawSprite()
{
	// 上下の黒帯（レターボックス）。intensity が上がるほど縮む。
	const float barH = TensionConst::kMaxBarHeight * (1.0f - m_intensity);
	if (barH < 0.5f) { return; }	// 完全に縮んだら描かない

	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

	// ウィンドウは1280x720。中心原点・+Y上。上端 y=+360／下端 y=-360。
	const int h = static_cast<int>(barH);
	const int topY = static_cast<int>(360.0f - barH * 0.5f);	// 上帯の中心Y
	sprite.DrawTex(&m_blackTex, 0,  topY, TensionConst::kBarWidth, h);	// 上
	sprite.DrawTex(&m_blackTex, 0, -topY, TensionConst::kBarWidth, h);	// 下

	KdShaderManager::Instance().UndoBlendState();
}
