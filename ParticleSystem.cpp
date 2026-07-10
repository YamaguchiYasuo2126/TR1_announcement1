#include "ParticleSystem.h"
#include "NoviceDrawUtility.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include <Novice.h>


void ParticleSystem::Initialize()
{
}

void ParticleSystem::Reset()
{
	for (int i = 0; i < kMaxParticles; i++)
	{
		// ポインタがnullptrでなければ非アクティブ化
		if (particles_[i] != nullptr)
		{
			particles_[i]->isActive = false;
		}
	}
}

void ParticleSystem::Update()
{
	float deltaTime = 1.0f / 60.0f;

	float sleepVelocityThreshold = 5.0f; // スリープと判定する速度の閾値
	float timeToSleep = 0.5f;            // スリープに入るまでの秒数（0.5秒動かなければスリープ）

	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		// 現在の速度の大きさ（の2乗）を計算（平方根の計算を省くための軽量化）
		float speedSq = particles_[i]->velocity.x * particles_[i]->velocity.x + particles_[i]->velocity.y * particles_[i]->velocity.y;

		if (speedSq < sleepVelocityThreshold * sleepVelocityThreshold)
		{
			// 速度が閾値以下なら、restTimeを増やす
			particles_[i]->restTime += deltaTime;
			if (particles_[i]->restTime > timeToSleep)
			{
				particles_[i]->isSleeping = true;
				// スリープに入ったら完全に停止させる
				particles_[i]->velocity = { 0.0f, 0.0f };
			}
		}
		else
		{
			// 速度が閾値を超えたら、スリープを解除してタイマーをリセット
			particles_[i]->restTime = 0.0f;
			particles_[i]->isSleeping = false;
		}
	}

	// コップ(境界)のパラメータ
	float cupLeft = cupPosition.x - cupWidth / 2.0f;
	float cupRight = cupPosition.x + cupWidth / 2.0f;
	float cupBottom = cupPosition.y + cupHeight / 2.0f;
	float cupTop = cupPosition.y - cupHeight / 2.0f;

	// 外力の適用と予測位置の計算
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive)
		{
			continue;
		}

		// スリープ状態でない（起きている）時だけ重力を加算する
		if (!particles_[i]->isSleeping)
		{
			// 重力(加速度)を速度に加算
			particles_[i]->velocity.x += particles_[i]->acceleration.x * deltaTime;
			particles_[i]->velocity.y += particles_[i]->acceleration.y * deltaTime;
		}

		// コップ外壁の吸着力（Adhesion）
		// パーティクルがコップの高さの範囲にいるか
		if (particles_[i]->position.y >= cupTop - 20.0f && particles_[i]->position.y <= cupBottom)
		{
			float adhesionDistance = 15.0f; // 吸着力を働かせる距離（パーティクルの半径＋αくらい）
			float adhesionForce = 350.0f;   // 壁に引き寄せる強さ

			// 左壁の外側にいる場合、右（壁側）へ引っ張る
			if (particles_[i]->position.x < cupLeft && particles_[i]->position.x > cupLeft - adhesionDistance)
			{
				particles_[i]->velocity.x += adhesionForce * deltaTime;
			}
			// 右壁の外側にいる場合、左（壁側）へ引っ張る
			else if (particles_[i]->position.x > cupRight && particles_[i]->position.x < cupRight + adhesionDistance)
			{
				particles_[i]->velocity.x -= adhesionForce * deltaTime;
			}
		}

		// 予測位置(predictedPosition)を計算
		particles_[i]->predictedPosition.x = particles_[i]->position.x + particles_[i]->velocity.x * deltaTime;
		particles_[i]->predictedPosition.y = particles_[i]->position.y + particles_[i]->velocity.y * deltaTime;


		// 画面全体の床の判定
		float screenFloor = 700.0f;
		if (particles_[i]->predictedPosition.y > screenFloor - particles_[i]->radius)
		{
			particles_[i]->predictedPosition.y = screenFloor - particles_[i]->radius;
			particles_[i]->velocity.y *= -0.2f; // 地面に着地
			particles_[i]->velocity.x *= 0.8f;  // 摩擦
		}
	}
	
	// 境界(コップ)との当たり判定（ローカル座標系への変換）
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		// 1. コップ中心からの相対位置ベクトル
		float dx = particles_[i]->predictedPosition.x - cupPosition.x;
		float dy = particles_[i]->predictedPosition.y - cupPosition.y;

		// 2. 逆回転させてローカル座標にする（角度をマイナスにして回転）
		float cosInv = cosf(-cupAngle);
		float sinInv = sinf(-cupAngle);
		float localX = dx * cosInv - dy * sinInv;
		float localY = dx * sinInv + dy * cosInv;

		// 速度ベクトルもローカル空間に変換しておく
		float localVelX = particles_[i]->velocity.x * cosInv - particles_[i]->velocity.y * sinInv;
		float localVelY = particles_[i]->velocity.x * sinInv + particles_[i]->velocity.y * cosInv;

		// 3. ローカル空間でのまっすぐなAABB判定
		float halfW = cupWidth / 2.0f;
		float halfH = cupHeight / 2.0f;
		float radius = particles_[i]->radius;
		bool isHit = false;

		// 左壁
		// localY が -halfH(コップの上端)より下にある時だけ壁として機能する
		if (localY >= -halfH && localX < -halfW + radius && localX > -halfW - 30.0f) {
			localX = -halfW + radius;
			localVelX *= -0.5f; // 反発係数
			isHit = true;
		}
		// 右壁
		// localY が -halfH(コップの上端)より下にある時だけ壁として機能する
		else if (localY >= -halfH && localX > halfW - radius && localX < halfW + 30.0f) {
			localX = halfW - radius;
			localVelX *= -0.5f;
			isHit = true;
		}

		// 底（Y軸下向き正）
		if (localX >= -halfW && localX <= halfW && localY > halfH - radius && localY < halfH + 30.0f) {
			localY = halfH - radius;
			localVelY *= -0.5f;
			isHit = true;
		}

		// 衝突していたら、ワールド座標に順回転で戻して適用
		if (isHit) {
			float cosFwd = cosf(cupAngle);
			float sinFwd = sinf(cupAngle);

			particles_[i]->predictedPosition.x = cupPosition.x + (localX * cosFwd - localY * sinFwd);
			particles_[i]->predictedPosition.y = cupPosition.y + (localX * sinFwd + localY * cosFwd);

			particles_[i]->velocity.x = localVelX * cosFwd - localVelY * sinFwd;
			particles_[i]->velocity.y = localVelX * sinFwd + localVelY * cosFwd;
		}
	}

	// 全粒子をグリッド（近傍探索用）に登録
	for (int i = 0; i < kGridWidth * kGridHeight; i++) {
		grid_[i].clear(); // 前フレームのデータを消去
	}
	for (int i = 0; i < kMaxParticles; i++) {
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		int gridX = static_cast<int>(particles_[i]->predictedPosition.x / kCellSize);
		int gridY = static_cast<int>(particles_[i]->predictedPosition.y / kCellSize);

		if (gridX < 0) gridX = 0;
		if (gridX >= kGridWidth) gridX = kGridWidth - 1;
		if (gridY < 0) gridY = 0;
		if (gridY >= kGridHeight) gridY = kGridHeight - 1;

		grid_[gridY * kGridWidth + gridX].push_back(i);
	}

	// 粒子同士の衝突解決(密度の計算と押し出し)
	// PBF(流体)のパラメータ
	// この値をいじるとドロドロ、サラサラなどの質感が変わる
	float smoothingRadius = 30.0f;    // 粒子がお互いを認識する半径(カーネル半径)
	float targetDensity = 26000.0f;  // 目標密度(この値に近づくように反発する)
	float pressureMultiplier = 0.1f;  // 押し出す力の強さ
	float smoothingRadiusSq = smoothingRadius * smoothingRadius;

	// 密度の計算(グリッドを使用して周辺9マスを探索)
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		float density = 0.0f;
		int myGridX = static_cast<int>(particles_[i]->predictedPosition.x / kCellSize);
		int myGridY = static_cast<int>(particles_[i]->predictedPosition.y / kCellSize);

		// 画面外クランプ
		if (myGridX < 0) myGridX = 0; if (myGridX >= kGridWidth) myGridX = kGridWidth - 1;
		if (myGridY < 0) myGridY = 0; if (myGridY >= kGridHeight) myGridY = kGridHeight - 1;

		for (int offsetY = -1; offsetY <= 1; offsetY++) 
		{
			for (int offsetX = -1; offsetX <= 1; offsetX++) 
			{
				int targetGridX = myGridX + offsetX;
				int targetGridY = myGridY + offsetY;
				if (targetGridX < 0 || targetGridX >= kGridWidth || targetGridY < 0 || targetGridY >= kGridHeight) continue;

				for (int j : grid_[targetGridY * kGridWidth + targetGridX])
				{
					if (particles_[j] == nullptr || !particles_[j]->isActive)
					{
						continue;
					}

					float dx = particles_[i]->predictedPosition.x - particles_[j]->predictedPosition.x;
					float dy = particles_[i]->predictedPosition.y - particles_[j]->predictedPosition.y;
					float distSq = dx * dx + dy * dy;

					// smoothingRadiusの範囲内にいる粒子だけを計算
					if (distSq < smoothingRadiusSq)
					{
						float dist = sqrtf(distSq);
						float influence = smoothingRadius - dist; // 近いほど値が大きくなる

						// 影響度を3乗して密度に加算(中心に近いほど急激に密度が上がる)
						density += influence * influence * influence;
					}
				}
			}
		}
		// 自分の密度として保存
		particles_[i]->density = density;
	}


	// 密度を元に「圧力」を計算し、予測位置を押し戻す
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive)
		{
			continue;
		}

		Vector2 pushVelocity = { 0.0f, 0.0f };

		// 自分の圧力 = (現在の密度 - 目標密度) * 圧力係数
		// 密度が目標より高ければプラス(反発)、低ければマイナス(引き合う)になる
		float pressureI = (particles_[i]->density - targetDensity) * pressureMultiplier;

		int myGridX = static_cast<int>(particles_[i]->predictedPosition.x / kCellSize);
		int myGridY = static_cast<int>(particles_[i]->predictedPosition.y / kCellSize);
		if (myGridX < 0) myGridX = 0; if (myGridX >= kGridWidth) myGridX = kGridWidth - 1;
		if (myGridY < 0) myGridY = 0; if (myGridY >= kGridHeight) myGridY = kGridHeight - 1;

		for (int offsetY = -1; offsetY <= 1; offsetY++)
		{
			for (int offsetX = -1; offsetX <= 1; offsetX++)
			{
				int targetGridX = myGridX + offsetX;
				int targetGridY = myGridY + offsetY;
				if (targetGridX < 0 || targetGridX >= kGridWidth || targetGridY < 0 || targetGridY >= kGridHeight) continue;

				for (int j : grid_[targetGridY * kGridWidth + targetGridX])
				{
					if (i == j)
					{
						continue; // 自分自身は弾かない
					}

					if (particles_[j] == nullptr || !particles_[j]->isActive)
					{
						continue;
					}

					float dx = particles_[i]->predictedPosition.x - particles_[j]->predictedPosition.x;
					float dy = particles_[i]->predictedPosition.y - particles_[j]->predictedPosition.y;
					float distSq = dx * dx + dy * dy;

					if (distSq < smoothingRadius * smoothingRadius)
					{
						float dist = sqrtf(distSq);
						if (dist < 0.0001f)
						{
							continue; // 完全に重なっている場合のゼロ除算を回避
						}

						// 相手の圧力
						float pressureJ = (particles_[j]->density - targetDensity) * pressureMultiplier;

						// お互いの圧力を平均化する(作用・反作用の法則)
						float sharedPressure = (pressureI + pressureJ) * 0.5f;

						// 爆発を防ぐため、圧力の計算結果は「押し出し(プラス)」のみに制限する
						if (sharedPressure < 0.0f)
						{
							sharedPressure = 0.0f;
						}

						// 押し出す強さの計算(近いほど強く押し出す)
						float influence = smoothingRadius - dist;
						float pushForce = sharedPressure * (influence * influence) / particles_[j]->density;

						// 表面張力(引力)の追加
						float tensionForce = 0.0f;
						float r = particles_[i]->radius;

						// 表面張力の強さ
						// 大きくするほど強くまとまり「スライム」や「水銀」のようになる
						float tensionStrength = 0.1f;

						// 粒子がめり込んでいる時は反発を優先し、
						// 「半径(r)よりは離れているが、影響半径(smoothingRadius)の範囲内にいる」時だけ引き合う
						if (dist > r && dist < smoothingRadius)
						{
							// 引力として働くようにマイナスの値にする
							tensionForce = -tensionStrength * influence;
						}

						// 最終的な移動力の決定
						// 反発力（＋）と 表面張力（－）を合算する
						float totalForce = pushForce + tensionForce;

						// 距離ベクトルを正規化して力を掛け、移動量に足し込む
						pushVelocity.x += (dx / dist) * totalForce;
						pushVelocity.y += (dy / dist) * totalForce;
					}
				}
			}
		}

		// 自分がスリープ状態なら、周囲からの反発力をゼロにする
		if (particles_[i]->isSleeping)
		{
			// 計算された反発力(pushVelocity)の大きさを確認
			float pushSq = pushVelocity.x * pushVelocity.x + pushVelocity.y * pushVelocity.y;

			// 目を覚ます力の閾値（衝撃への敏感さ）
			float wakeUpThreshold = 100.0f;

			if (pushSq > wakeUpThreshold * wakeUpThreshold)
			{
				// 強い力（衝撃）を受けたので目を覚ます
				particles_[i]->isSleeping = false;
				particles_[i]->restTime = 0.0f;
			}
			else
			{
				// 力が弱ければ、そのまま静止し続ける
				pushVelocity = { 0.0f, 0.0f };
			}
		}

		// 計算した反発力を使って、予測位置をずらす
		particles_[i]->predictedPosition.x += pushVelocity.x * deltaTime;
		particles_[i]->predictedPosition.y += pushVelocity.y * deltaTime;
	}

	// 境界(コップ)との当たり判定2回目
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive)
		{
			continue;
		}
		
		// 底面(y = cupBottom)の判定
		// 粒子の横幅がコップの底面の範囲(cupLeft 〜 cupRight)にあるとき
		if (particles_[i]->predictedPosition.x >= cupLeft && particles_[i]->predictedPosition.x <= cupRight)
		{
			// 内側(上)から底面にめり込んだ場合
			// 中心が底面より上にあり、かつ下端が底面を突き抜けている
			if (particles_[i]->predictedPosition.y > cupBottom - particles_[i]->radius && particles_[i]->predictedPosition.y < cupBottom)
			{
				particles_[i]->predictedPosition.y = cupBottom - particles_[i]->radius;
				particles_[i]->velocity.y *= -0.1f; // 反発
			}
			// 外側(下)から底面にめり込んだ場合
			// 中心が底面より下にあり、かつ上端が底面を突き抜けている
			else if (particles_[i]->predictedPosition.y < cupBottom + particles_[i]->radius && particles_[i]->predictedPosition.y >= cupBottom)
			{
				particles_[i]->predictedPosition.y = cupBottom + particles_[i]->radius;
				particles_[i]->velocity.y *= -0.1f;
			}
		}

		// 側面(cupLeft, cupRight)の判定
		// 粒子の縦幅がコップの高さの範囲(cupTop 〜 cupBottom)にあるとき
		if (particles_[i]->predictedPosition.y >= cupTop && particles_[i]->predictedPosition.y <= cupBottom)
		{
			// 左壁(x = cupLeft)の判定
			// (内側(右)から左壁にめり込んだ場合
			if (particles_[i]->predictedPosition.x < cupLeft + particles_[i]->radius && particles_[i]->predictedPosition.x > cupLeft)
			{
				particles_[i]->predictedPosition.x = cupLeft + particles_[i]->radius;
				particles_[i]->velocity.x *= -0.1f;
			}
			// 外側(左)から左壁にめり込んだ場合
			else if (particles_[i]->predictedPosition.x > cupLeft - particles_[i]->radius && particles_[i]->predictedPosition.x <= cupLeft)
			{
				particles_[i]->predictedPosition.x = cupLeft - particles_[i]->radius;
				particles_[i]->velocity.x *= 0.0f;  // 反発させず、壁にピタッとくっつける
				particles_[i]->velocity.y *= 0.8f;  // 落下速度を減衰させて伝うようにする
			}

			// 右壁(x = cupRight)の判定
			// 内側(左)から右壁にめり込んだ場合
			if (particles_[i]->predictedPosition.x > cupRight - particles_[i]->radius && particles_[i]->predictedPosition.x < cupRight)
			{
				particles_[i]->predictedPosition.x = cupRight - particles_[i]->radius;
				particles_[i]->velocity.x *= -0.1f;
			}
			// 外側(右)から右壁にめり込んだ場合
			else if (particles_[i]->predictedPosition.x < cupRight + particles_[i]->radius && particles_[i]->predictedPosition.x >= cupRight)
			{
				particles_[i]->predictedPosition.x = cupRight + particles_[i]->radius;
				particles_[i]->velocity.x *= 0.0f;  // 反発させず、壁にピタッとくっつける
				particles_[i]->velocity.y *= 0.8f;  // 落下速度を減衰させて伝うようにする
			}
		}
	}

	// 速度の再計算と位置の確定
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive)
		{
			continue;
		}

		// 修正された予測位置から、実際の速度を逆算する
		particles_[i]->velocity.x = (particles_[i]->predictedPosition.x - particles_[i]->position.x) / deltaTime;
		particles_[i]->velocity.y = (particles_[i]->predictedPosition.y - particles_[i]->position.y) / deltaTime;

		// コップのふち（エッジ）での爆発的な横飛びを強力に抑える
		// ふちの高さ付近(cupTop)にいるパーティクルを対象
		if (particles_[i]->predictedPosition.y >= cupTop - particles_[i]->radius * 2.0f &&
			particles_[i]->predictedPosition.y <= cupTop + particles_[i]->radius * 2.0f)
		{
			// 左のふち付近で、左（外側）へ強く飛ぼうとしている場合
			if (particles_[i]->predictedPosition.x < cupLeft &&
				particles_[i]->predictedPosition.x > cupLeft - 20.0f &&
				particles_[i]->velocity.x < 0.0f) // 速度がマイナス(左向き)の時だけ
			{
				particles_[i]->velocity.x *= 0.5f; // 横方向の勢いを50%カットして殺す
				particles_[i]->velocity.y += 200.0f * deltaTime; // 少し下向きに落として吸着力(Adhesion)エリアへ誘導

				// 変更した速度に合わせて予測位置を再計算
				particles_[i]->predictedPosition.x = particles_[i]->position.x + particles_[i]->velocity.x * deltaTime;
				particles_[i]->predictedPosition.y = particles_[i]->position.y + particles_[i]->velocity.y * deltaTime;
			}
			// 右のふち付近で、右（外側）へ強く飛ぼうとしている場合
			else if (particles_[i]->predictedPosition.x > cupRight &&
				particles_[i]->predictedPosition.x < cupRight + 20.0f &&
				particles_[i]->velocity.x > 0.0f) // 速度がプラス(右向き)の時だけ
			{
				particles_[i]->velocity.x *= 0.5f; // 横方向の勢いを50%カット
				particles_[i]->velocity.y += 200.0f * deltaTime;

				// 変更した速度に合わせて予測位置を再計算
				particles_[i]->predictedPosition.x = particles_[i]->position.x + particles_[i]->velocity.x * deltaTime;
				particles_[i]->predictedPosition.y = particles_[i]->position.y + particles_[i]->velocity.y * deltaTime;
			}
		}

		// 速度を少し減衰させる
		particles_[i]->velocity.x *= 0.98f;
		particles_[i]->velocity.y *= 0.98f;

		// 速度リミッター(異常な吹き飛びを防止)
		float maxSpeed = 800.0f;
		float speedSq = (particles_[i]->velocity.x * particles_[i]->velocity.x) + (particles_[i]->velocity.y * particles_[i]->velocity.y);

		// 速度の2乗が最大速度の2乗を超えていたら制限をかける
		if (speedSq > maxSpeed * maxSpeed)
		{
			float currentSpeed = sqrtf(speedSq);
			// 速度の方向を保ったまま、長さをmaxSpeedに縮小する
			particles_[i]->velocity.x = (particles_[i]->velocity.x / currentSpeed) * maxSpeed;
			particles_[i]->velocity.y = (particles_[i]->velocity.y / currentSpeed) * maxSpeed;

			// 予測位置も制限された速度に合わせて再計算しておく
			particles_[i]->predictedPosition.x = particles_[i]->position.x + particles_[i]->velocity.x * deltaTime;
			particles_[i]->predictedPosition.y = particles_[i]->position.y + particles_[i]->velocity.y * deltaTime;
		}

		// 位置を確定させる
		particles_[i]->position = particles_[i]->predictedPosition;

		// 経過時間を更新
		particles_[i]->elapsedTime += deltaTime;
	}

}

void ParticleSystem::Draw()
{

	// コップのローカル頂点（左上、左下、右下、右上）
	float halfW = cupWidth / 2.0f;
	float halfH = cupHeight / 2.0f;
	Vector2 localTL = { -halfW, -halfH };
	Vector2 localBL = { -halfW,  halfH };
	Vector2 localBR = { halfW,  halfH };
	Vector2 localTR = { halfW, -halfH };

	// ワールド座標に変換するラムダ式（コードをすっきりさせるため）
	auto toWorld = [&](Vector2 local) -> Vector2 {
		return {
			cupPosition.x + (local.x * cosf(cupAngle) - local.y * sinf(cupAngle)),
			cupPosition.y + (local.x * sinf(cupAngle) + local.y * cosf(cupAngle))
		};
		};

	Vector2 worldTL = toWorld(localTL);
	Vector2 worldBL = toWorld(localBL);
	Vector2 worldBR = toWorld(localBR);
	Vector2 worldTR = toWorld(localTR);

	// 左壁・底・右壁の3本の線でコップを描画
	unsigned int cupColor = 0x555555FF;
	Novice::DrawLine((int)worldTL.x, (int)worldTL.y, (int)worldBL.x, (int)worldBL.y, cupColor);
	Novice::DrawLine((int)worldBL.x, (int)worldBL.y, (int)worldBR.x, (int)worldBR.y, cupColor);
	Novice::DrawLine((int)worldBR.x, (int)worldBR.y, (int)worldTR.x, (int)worldTR.y, cupColor);

	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr)
		{

			continue;

		}

		if (!particles_[i]->isActive)
		{
			continue;

		}

		

		DrawCircle(particles_[i]->position, particles_[i]->radius, particles_[i]->color);

	}

}

void ParticleSystem::Finalize()
{
}

void ParticleSystem::Emit(const Vector2& position, const Vector2& velocity, const Vector2& acceleration, float radius, unsigned int color, int isBubble)
{
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr)
		{
			particles_[i] = new Particle;
		}

		if (particles_[i]->isActive)
		{
			continue;
		}

		particles_[i]->isActive = true;
		particles_[i]->position = position;
		particles_[i]->velocity = velocity;
		particles_[i]->initialVelocity = velocity;
		particles_[i]->acceleration = acceleration;
		particles_[i]->radius = radius;
		particles_[i]->color = color;
		particles_[i]->elapsedTime = 0.0f;
		particles_[i]->isBubble = isBubble;
		
		break;
	}
}


