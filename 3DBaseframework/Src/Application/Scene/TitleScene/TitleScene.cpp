#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../GameObject/Water/FluidField.h"
#include "../../GameObject/Water/WaterConst.h"
#include "../../main.h"

namespace
{
	// 背景の流体表示サイズ（ウィンドウ1280x720を覆う）
	constexpr int kFluidWidth  = 1280;
	constexpr int kFluidHeight = 720;

	// 背景流体のグリッド解像度。画面と同じ16:9にして引き伸ばしによる粗さを無くす。
	//  （ゲーム本編の縦長グリッドを16:9へ引き伸ばすと解像度が低く見えるため専用に持つ）
	constexpr int kFluidGridW = 480;
	constexpr int kFluidGridH = 270;	// 480:270 = 16:9

	// 背景流体の初期水量（下からの割合）と、波立たせる注水の周期
	constexpr float kFluidInitialFill = 0.35f;
	constexpr float kPourCycleSec     = 3.0f;	// この周期で
	constexpr float kPourDurationSec  = 0.8f;	// このあいだ注ぐ
	constexpr float kFluidResetFill   = 0.92f;	// 溜まりすぎたらリセット
}

void TitleScene::Draw()
{
	//仮で背景を白色に
	Math::Color color = { 1.0f,1.0f,1.0f,1.0f };
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 640, 360, &color, true);

	//===================　背景の流体（旧サイン波画像の代わり）===================
	// GPU流体シミュレーションの可視化テクスチャを全画面に貼る。
	//  白背景の上に半透明の水が乗るので、水面の揺れがそのまま見える。
	if (m_fluid)
	{
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		KdShaderManager::Instance().ChangeSamplerState(KdSamplerState::Linear_Clamp);
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_fluid->GetDisplayTexture().get(), 0, 0, kFluidWidth, kFluidHeight);
		KdShaderManager::Instance().UndoSamplerState();
		KdShaderManager::Instance().UndoBlendState();
	}

	//======================　タイトル表示　=========================
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_titleTex, 0, 100, 512, 256);


	//====================== プレスキー表示　========================
	Math::Rectangle rc = { 0, 0, 512, 256 };
	Math::Color spaceColor = { 1.0f,1.0f,1.0f,m_alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex
	(
		&m_spaceTex, 0, -180, 512, 256, &rc, &spaceColor
	);
}

void TitleScene::Update()
{
	//背景の流体を進める。一定周期で少しだけ注いで、水面を波立たせ続ける。
	if (m_fluid)
	{
		float deltaTime = Application::Instance().GetDeltaTime();
		if (deltaTime > WaterConst::kMaxDeltaTime) { deltaTime = WaterConst::kMaxDeltaTime; }

		m_fluidTime += deltaTime;

		// 溜まりすぎたら初期状態へ戻す（あふれ続けないように）
		if (m_fluid->GetFillRate() > kFluidResetFill)
		{
			m_fluid->Reset();
			m_fluidTime = 0.0f;
		}

		// kPourCycleSec 周期の先頭 kPourDurationSec だけ注ぐ＝定期的に水しぶきが立つ
		const float phase = std::fmod(m_fluidTime, kPourCycleSec);
		const bool  pour  = (phase < kPourDurationSec);
		m_fluid->Step(deltaTime, pour);
	}

	//プレスキー表示の点滅
	m_alpha += m_alphaAdd;

	if (m_alpha > 1.0f)
	{
		m_alpha = 1.0f;
		m_alphaAdd *= -1;
	}
	else if (m_alpha < 0.2f)
	{
		m_alpha = 0.2f;
		m_alphaAdd *= -1;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}

void TitleScene::Event()
{

}

void TitleScene::Init()
{
	KdAudioManager::Instance().Play("Asset//Data/Audio/title.WAV", true);

	m_spaceTex.Load("Asset/Textures/space.png");
	m_titleTex.Load("Asset/Textures/title_kari.png");

	//背景の流体：コップ形状ではなく「全面の水槽」にして、下から少し水を張った状態で開始する
	m_fluid = std::make_shared<FluidField>();
	m_fluid->SetGridSize(kFluidGridW, kFluidGridH);	// 画面と同じ16:9の解像度で持つ
	m_fluid->SetFullRectMask(true);
	m_fluid->SetInitialFill(kFluidInitialFill);
	m_fluid->Init();
	m_fluidTime = 0.0f;
}
