#include <Novice.h>
#include <stdlib.h>
#include "Vector2.h"
#include "NoviceDrawUtility.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "Player.h"
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

	Vector2 emitterPos = { 640.0f, 360.0f };

	particleEmitter.velocity_ = { 0.0f, -100.0f };

	particleEmitter.acceleration_ = { 0.0f, -0.4f };

	// 隕石用の変数
	Vector2 meteorPos = { 100.0f, -100.0f };
	Vector2 meteorVelocity = { 8.0f, 8.0f };
	bool isMeteorAlive = false;

	particleEmitter.lifeTime_ = 2.0f;
	particleEmitter.color_ = 0xE0F7FFFF;
	const float emitterMoveSpeed = 5.0f;

	float torchMoveTimer = 0.0f;
	const float kTorchLimit = 3.0f;

	Player player;
	player.Initialize(&particleSystem);

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

		if (keys[DIK_2] && !preKeys[DIK_2])
		{
			currentScene = TORCH;
		}

		if (keys[DIK_3] && !preKeys[DIK_3])
		{
			currentScene = FOG;
		}

		if (keys[DIK_4] && !preKeys[DIK_4])
		{
			currentScene = METEOR;
		}


		// シーンが変わった瞬間だけの処理(設定の変更)
		if (currentScene != preScene)
		{
			particleSystem.Reset();

			switch (currentScene)
			{
			case BUBBLE:

				particleEmitter.isBubble_ = true;
				particleEmitter.isFire_ = false;
				particleEmitter.isFog_ = false;
				// 泡の設定
				particleEmitter.SetEmitterConfig({ 0.0f, -100.0f }, { 0.0f, -0.4f }, 4.0f, 0xE0F7FFFF, { 200.0f, 100.0f }, 2.0f, 16.0f);
				emitterPos = { 640.0f, 360.0f };
				break;

			case TORCH:

				particleEmitter.isBubble_ = false;
				particleEmitter.isFire_ = true;
				particleEmitter.isFog_ = false;
				// 松明の設定
				particleEmitter.SetEmitterConfig({ 0.0f, -150.0f }, { 0.0f, 2.0f }, 1.5f, 0xFF4500FF, { 20.0f, 10.0f }, 32.0f, 48.0f);
				emitterPos = { 640.0f, 360.0f };
				break;

			case FOG:

				particleEmitter.isBubble_ = false;
				particleEmitter.isFire_ = false;
				particleEmitter.isFog_ = true;
				// 霧の設定
				particleEmitter.SetEmitterConfig({ 20.0f, 0.0f }, { 0.0f, 0.0f }, 8.0f, 0xFFFFFF20, { 600.0f, 200.0f }, 40.0f, 80.0f);
				emitterPos = { 640.0f, 360.0f };
				break;

			case METEOR:
				particleEmitter.isBubble_ = false;
				particleEmitter.isFire_ = false;
				particleEmitter.isFog_ = false;

				// 隕石の後ろに出る煙の設定
				particleEmitter.SetEmitterConfig({ -3.0f, -3.0f }, { 0.0f, 0.0f }, 0.5f, 0x808080FF, { 20.0f, 20.0f }, 10.0f, 20.0f);

				// 隕石の位置をリセット
				meteorPos = { 100.0f, -100.0f };
				isMeteorAlive = true;
				break;
			}
			preScene = currentScene; // 更新完了
		}

		int isEmitterMoving = false;

		if (keys[DIK_T])
		{
			emitterPos.y -= emitterMoveSpeed;
			if (currentScene == TORCH)
			{
				isEmitterMoving = true;
			}
		}

		if (keys[DIK_G])
		{
			emitterPos.y += emitterMoveSpeed;
			if (currentScene == TORCH)
			{
				isEmitterMoving = true;
			}
		}

		if (keys[DIK_F])
		{
			emitterPos.x -= emitterMoveSpeed;
			if (currentScene == TORCH)
			{
				isEmitterMoving = true;
			}
		}

		if (keys[DIK_H])
		{
			emitterPos.x += emitterMoveSpeed;
			if (currentScene == TORCH)
			{
				isEmitterMoving = true;
			}
		}

		// リセット処理
		if (keys[DIK_R] && !preKeys[DIK_R])
		{
			particleSystem.Reset();

			// 隕石シーンなら隕石もリセット
			if (currentScene == METEOR)
			{
				meteorPos = { 100.0f, -100.0f };
				isMeteorAlive = true;
			}
		}

		if (isEmitterMoving)
		{
			// 動いているなら時間を加算
			torchMoveTimer += 1.0f / 60.0f;
		}
		else
		{
			// 徐々に炎が復活するようにする
			torchMoveTimer -= 1.0f / 60.0f;
			if (torchMoveTimer < 0.0f)
			{
				torchMoveTimer = 0.0f;
			}
		}

		player.Update(keys);

		if (currentScene == METEOR)
		{
			if (isMeteorAlive)
			{
				// 移動
				meteorPos.x += meteorVelocity.x;
				meteorPos.y += meteorVelocity.y;

				// 煙を出すためにエミッターを隕石の位置へ
				emitterPos = meteorPos;

				// 地面との衝突判定
				if (meteorPos.y >= 700.0f)
				{
					isMeteorAlive = false; // 隕石消滅

					// 爆発エフェクト
					for (int i = 0; i < 600; i++)
					{
						// ランダムな角度と速度
						int degree = rand() % 360;
						float angle = degree * static_cast<float>(M_PI) / 180.0f;
						float speed = 640.0f + static_cast<float>(rand() % 200);

						Vector2 explodeVelocity;
						explodeVelocity.x = cosf(angle) * speed;
						explodeVelocity.y = sinf(angle) * speed;

						// ランダムな色
						unsigned int r = rand() % 256;
						unsigned int g = rand() % 256;
						unsigned int b = rand() % 256;
						unsigned int randomColor = (r << 24) | (g << 16) | (b << 8) | 0xFF;

						// パーティクルシステムに直接登録
						particleSystem.Emit(meteorPos, explodeVelocity, { 0.0f, 0.5f }, 1.0f, 10.0f, randomColor, false, false, false);
					}
				}
			}
		}

		particleEmitter.SetPosition(emitterPos);

		// パーティクルシステム更新
		particleSystem.Update();

		if (currentScene == TORCH)
		{
			// 松明シーンの場合
			if (torchMoveTimer < kTorchLimit)
			{
				particleEmitter.Emit();
			}
		}
		else if (currentScene == METEOR)
		{
			if (isMeteorAlive)
			{
				particleEmitter.Emit();
			}
		}
		else
		{
			// 他のシーンは常に出す
			particleEmitter.Emit();
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::ScreenPrintf(40, 20, "PLAYER MOVE : WASD or ARROW KEY");
		Novice::ScreenPrintf(40, 60, "EMITTER MOVE : TFGH KEY");
		Novice::ScreenPrintf(40, 100, "SCENE CHANGE : 1 or 2 or 3 or 4 KEY");
		Novice::ScreenPrintf(40, 140, "RESET : R KEY");
		if (currentScene == BUBBLE)
		{
			Novice::ScreenPrintf(40, 180, "CURRENT SCENE : BUBLE");
		}
		else if (currentScene == TORCH)
		{
			Novice::ScreenPrintf(40, 180, "CURRENT SCENE : TORCH");
			Novice::ScreenPrintf(40, 220, "3 seconds move ? : %d", static_cast<int>(torchMoveTimer));
		}
		else if (currentScene == FOG)
		{
			Novice::ScreenPrintf(40, 180, "CURRENT SCENE : FOG");
		}
		else if (currentScene == METEOR)
		{
			Novice::ScreenPrintf(40, 180, "CURRENT SCENE : METEOR");
		}


		// 背景色をシーンによって変える
		unsigned int bgColor = 0x000000FF;
		if (currentScene == BUBBLE)
		{
			bgColor = 0xADD8E699;
		}
		else if (currentScene == TORCH)
		{
			bgColor = 0x111122FF;
		}
		else if (currentScene == FOG)
		{
			bgColor = 0x00CC9980;
		}
		else if (currentScene == METEOR)
		{
			bgColor = 0x000033FF;
		}

		Novice::DrawBox
		(
			0, 0, 1280, 720, 0.0f, bgColor, kFillModeSolid
		);

		if (currentScene == TORCH)
		{
			// 松明の持ち手を描画
			Novice::DrawBox(
				static_cast<int>(emitterPos.x) - 50, static_cast<int>(emitterPos.y),
				100, 300, 0.0f, 0x8B4513FF, kFillModeSolid // 茶色
			);

			// 加算合成を開始
			Novice::SetBlendMode(BlendMode::kBlendModeAdd);

			// 炎のパーティクルを描画
			particleSystem.Draw();

			// 加算合成を終了
			Novice::SetBlendMode(BlendMode::kBlendModeNormal);
		}
		else if (currentScene == METEOR) // ★追加：隕石シーンの描画
		{
			// 隕石本体(生きていれば)
			if (isMeteorAlive)
			{
				Novice::DrawEllipse(static_cast<int>(meteorPos.x), static_cast<int>(meteorPos.y), 20, 20, 0.0f, 0xFF4500FF, kFillModeSolid);
			}

			// 爆発と煙(加算合成できれいに見せる)
			Novice::SetBlendMode(BlendMode::kBlendModeAdd);
			particleSystem.Draw();
			Novice::SetBlendMode(BlendMode::kBlendModeNormal);
		}
		else
		{
			// その他のシーンは普通に描画
			particleSystem.Draw();
			particleEmitter.Draw();
		}

		player.Draw();



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
