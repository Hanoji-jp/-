#include "Tension.h"
#include "TensionConst.h"
#include "../Camera/CameraBase.h"
#include "../Water/Water.h"
#include "../Cup/CupConst.h"
#include "../GameStart/GameStart.h"
#include <cmath>	// 手持ちカメラ風の揺れ（sin/cos）
#include <fstream>	// 音声ファイルの存在確認

namespace
{
	// 置いてある音声だけをループ再生する。
	//  KdAudioManager は存在しないファイルを渡すとアサートで落ちるため、先に存在を確認する。
	//  （籠りBGMを後から追加できるようにするための保険）
	std::shared_ptr<KdSoundInstance> PlayLoopIfExists(const char* path)
	{
		if (!path) { return nullptr; }

		std::ifstream file(path, std::ios::binary);
		if (!file.is_open()) { return nullptr; }
		file.close();

		return KdAudioManager::Instance().Play(path, true);
	}
}

void Tension::Init()
{
	m_blackTex.Load(TensionConst::kBlackImage);
	// 心臓音は開始演出（3・2・1・start）が終わってから鳴らす（Update で開始）。
	m_started   = false;
	m_intensity = 0.0f;

	// BGMも心臓音と同じく、開始演出（3・2・1・始めっ）が終わってから鳴らす（Update で開始）。
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

		// BGMも同時に鳴らし始める（普通から。水が溜まるほど籠り側へクロスフェード）。
		//  ※存在しないファイルを Play するとアサートで落ちるので、置いてある物だけ鳴らす。
		m_bgm = PlayLoopIfExists(TensionConst::kBgmAudio);
		if (m_bgm) { m_bgm->SetVolume(TensionConst::kBgmVolume); }

		// 籠りBGM（未配置なら null のまま＝普通のBGMを絞るだけの挙動になる）
		m_bgmMuffled = PlayLoopIfExists(TensionConst::kBgmMuffledAudio);
		if (m_bgmMuffled) { m_bgmMuffled->SetVolume(0.0f); }

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

	// BGM：水が溜まるほど「普通→籠り」へクロスフェード（合計音量は一定に保つ）。
	if (m_bgmMuffled)
	{
		if (m_bgm) { m_bgm->SetVolume(TensionConst::kBgmVolume * (1.0f - m_intensity)); }
		m_bgmMuffled->SetVolume(TensionConst::kBgmVolume * m_intensity);
	}
	else if (m_bgm)
	{
		// 籠りBGMが未配置のとき：普通のBGMを絞って引っ込ませるだけにする。
		m_bgm->SetVolume(TensionConst::kBgmVolume
			+ (TensionConst::kBgmVolumeNoMuffled - TensionConst::kBgmVolume) * m_intensity);
	}

	// 手持ちカメラ風の揺れ：周波数の違う正弦波を重ねて不規則に見せる。
	//  緊張が高いほど（目標に近づくほど）揺れを少し大きくする。
	m_swayTime += TensionConst::kSwaySpeed;
	const float swayGain = 1.0f + (TensionConst::kSwayIntensityGain - 1.0f) * m_intensity;
	const float swayX = (std::sinf(m_swayTime * 1.00f) * 0.6f
					   + std::sinf(m_swayTime * 2.30f) * 0.4f) * TensionConst::kSwayAmpX * swayGain;
	const float swayY = (std::cosf(m_swayTime * 1.30f) * 0.6f
					   + std::sinf(m_swayTime * 3.10f) * 0.4f) * TensionConst::kSwayAmpY * swayGain;

	// カメラをコップへズームイン（intensity ぶんだけ近づける）＋手持ち揺れ
	if (auto cam = m_wpCamera.lock())
	{
		const float z = -(CupConst::kCameraDistanceZ - m_intensity * TensionConst::kZoomAmount);
		cam->SetPos({ CupConst::kCenterX + swayX, swayY, z });
	}
}

void Tension::DrawSprite()
{
	// 上下の黒帯（レターボックス）。開始は薄く、目標に近づく（intensity↑）ほど閉じて太くなる。
	const float barH = TensionConst::kMinBarHeight
		+ (TensionConst::kMaxBarHeight - TensionConst::kMinBarHeight) * m_intensity;
	if (barH < 0.5f) { return; }

	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

	// ウィンドウは1280x720。中心原点・+Y上。上端 y=+360／下端 y=-360。
	//  画面外へ kBarOvershoot ぶんはみ出させて描く（整数丸めで端に1pxの隙間ができるのを防ぐ）。
	const float over = TensionConst::kBarOvershoot;
	const int   h    = static_cast<int>(barH + over);
	const int   cY   = static_cast<int>(TensionConst::kScreenHalfHeight - barH * 0.5f + over * 0.5f);
	sprite.DrawTex(&m_blackTex, 0,  cY, TensionConst::kBarWidth, h);	// 上
	sprite.DrawTex(&m_blackTex, 0, -cY, TensionConst::kBarWidth, h);	// 下

	KdShaderManager::Instance().UndoBlendState();
}
