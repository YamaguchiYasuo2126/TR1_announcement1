#include <Novice.h>
#include <stdlib.h>
#include "Vector2.h"
#include "NoviceDrawUtility.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LE2B_28_ヤマグチ_ヤスオ";

//==========================================
// 構造体の宣言
//==========================================

// シーン
enum Scene
{
	BUBBLE,
	TORCH,
	FOG,
	METEOR
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	ParticleSystem particleSystem{};

	particleSystem.Initialize();

	ParticleEmitter particleEmitter(&particleSystem);

	Vector2 emitterPos = { 640.0f, 100.0f };

	particleEmitter.velocity_ = { 0.0f, 100.0f };

	particleEmitter.acceleration_ = { 0.0f, 1500.0f };

	particleEmitter.color_ = 0xE0F7FFFF;
	const float emitterMoveSpeed = 5.0f;

	Scene currentScene = BUBBLE;

	// 前のシーン(最初は不一致にするため別の値を入れておく)
	Scene preScene = METEOR;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// 1. シーン切り替え入力
		if (keys[DIK_1] && !preKeys[DIK_1])
		{
			currentScene = BUBBLE;
		}


		// シーンが変わった瞬間だけの処理(設定の変更)
		if (currentScene != preScene)
		{
			particleSystem.Reset();

			switch (currentScene)
			{
			case BUBBLE:

				particleEmitter.isBubble_ = true;
				// 泡の設定
				particleEmitter.SetEmitterConfig({ 0.0f, 1000.0f }, { 0.0f, 256.0f }, 0x007DC5FF, { 100.0f, 100.0f }, 20.0f, 20.0f);
				emitterPos = { 640.0f, 100.0f };
				break;

			case TORCH:

				particleEmitter.isBubble_ = false;
				// 松明の設定
				particleEmitter.SetEmitterConfig({ 0.0f, -150.0f }, { 0.0f, 2.0f }, 0xFF4500FF, { 20.0f, 10.0f }, 32.0f, 48.0f);
				emitterPos = { 640.0f, 360.0f };
				break;

			case FOG:

				particleEmitter.isBubble_ = false;
				// 霧の設定
				particleEmitter.SetEmitterConfig({ 20.0f, 0.0f }, { 0.0f, 0.0f }, 0xFFFFFF20, { 600.0f, 200.0f }, 40.0f, 80.0f);
				emitterPos = { 640.0f, 360.0f };
				break;

			case METEOR:
				particleEmitter.isBubble_ = false;

				// 隕石の後ろに出る煙の設定
				particleEmitter.SetEmitterConfig({ -3.0f, -3.0f }, { 0.0f, 0.0f }, 0x808080FF, { 20.0f, 20.0f }, 10.0f, 20.0f);

				break;
			}
			preScene = currentScene; // 更新完了
		}

		if (keys[DIK_W])
		{
			emitterPos.y -= emitterMoveSpeed;
			
		}

		if (keys[DIK_S])
		{
			emitterPos.y += emitterMoveSpeed;
			
		}

		if (keys[DIK_A])
		{
			emitterPos.x -= emitterMoveSpeed;
			
		}

		if (keys[DIK_D])
		{
			emitterPos.x += emitterMoveSpeed;
			
		}

		// リセット処理
		if (keys[DIK_R] && !preKeys[DIK_R])
		{
			particleSystem.Reset();

		}

		particleEmitter.SetPosition(emitterPos);

		// パーティクルシステム更新
		particleSystem.Update();


		// 他のシーンは常に出す
		particleEmitter.Emit();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::ScreenPrintf(40, 20, "EMITTER MOVE : WASD KEY");
		Novice::ScreenPrintf(40, 60, "RESET : R KEY");

		// 背景色をシーンによって変える
		unsigned int bgColor = 0x000000FF;
		if (currentScene == BUBBLE)
		{
			bgColor = 0x111122FF;
		}

		Novice::DrawBox
		(
			0, 0, 1280, 720, 0.0f, bgColor, kFillModeSolid
		);


		// その他のシーンは普通に描画
		particleSystem.Draw();
		particleEmitter.Draw();

		///
		/// ↑描画処理ここまで
		///


		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
