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

	// コップ(境界)のパラメータ
	float cupLeft = 540.0f;
	float cupRight = 740.0f;
	float cupBottom = 600.0f;
	float cupTop = 300.0f;

	// 外力の適用と予測位置の計算
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive)
		{
			continue;
		}

		// 重力(加速度)を速度に加算
		particles_[i]->velocity.x += particles_[i]->acceleration.x * deltaTime;
		particles_[i]->velocity.y += particles_[i]->acceleration.y * deltaTime;

		// 予測位置(predictedPosition)を計算
		particles_[i]->predictedPosition.x = particles_[i]->position.x + particles_[i]->velocity.x * deltaTime;
		particles_[i]->predictedPosition.y = particles_[i]->position.y + particles_[i]->velocity.y * deltaTime;

		// 境界(コップ)との当たり判定

		// 底面(y = cupBottom)の判定
		// 粒子の横幅がコップの底面の範囲(cupLeft 〜 cupRight)にあるとき
		if (particles_[i]->predictedPosition.x >= cupLeft && particles_[i]->predictedPosition.x <= cupRight)
		{
			// 内側(上)から底面にめり込んだ場合
			// 中心が底面より上にあり、かつ下端が底面を突き抜けている
			if (particles_[i]->predictedPosition.y > cupBottom - particles_[i]->radius && particles_[i]->predictedPosition.y < cupBottom)
			{
				particles_[i]->predictedPosition.y = cupBottom - particles_[i]->radius;
				particles_[i]->velocity.y *= -0.5f; // 反発
			}
			// 外側(下)から底面にめり込んだ場合
			// 中心が底面より下にあり、かつ上端が底面を突き抜けている
			else if (particles_[i]->predictedPosition.y < cupBottom + particles_[i]->radius && particles_[i]->predictedPosition.y >= cupBottom)
			{
				particles_[i]->predictedPosition.y = cupBottom + particles_[i]->radius;
				particles_[i]->velocity.y *= -0.5f;
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
				particles_[i]->velocity.x *= -0.5f;
			}
			// 外側(左)から左壁にめり込んだ場合
			else if (particles_[i]->predictedPosition.x > cupLeft - particles_[i]->radius && particles_[i]->predictedPosition.x <= cupLeft)
			{
				particles_[i]->predictedPosition.x = cupLeft - particles_[i]->radius;
				particles_[i]->velocity.x *= -0.5f;
			}

			// 右壁(x = cupRight)の判定
			// 内側(左)から右壁にめり込んだ場合
			if (particles_[i]->predictedPosition.x > cupRight - particles_[i]->radius && particles_[i]->predictedPosition.x < cupRight)
			{
				particles_[i]->predictedPosition.x = cupRight - particles_[i]->radius;
				particles_[i]->velocity.x *= -0.5f;
			}
			// 外側(右)から右壁にめり込んだ場合
			else if (particles_[i]->predictedPosition.x < cupRight + particles_[i]->radius && particles_[i]->predictedPosition.x >= cupRight)
			{
				particles_[i]->predictedPosition.x = cupRight + particles_[i]->radius;
				particles_[i]->velocity.x *= -0.5f;
			}
		}

		// 画面全体の床の判定
		float screenFloor = 700.0f;
		if (particles_[i]->predictedPosition.y > screenFloor - particles_[i]->radius)
		{
			particles_[i]->predictedPosition.y = screenFloor - particles_[i]->radius;
			particles_[i]->velocity.y *= -0.2f; // 地面に着地
			particles_[i]->velocity.x *= 0.8f;  // 摩擦
		}
	}

	// 粒子同士の衝突解決(密度の計算と押し出し)
	// PBF(流体)のパラメータ
	// この値をいじるとドロドロ、サラサラなどの質感が変わる
	float smoothingRadius = 40.0f;    // 粒子がお互いを認識する半径(カーネル半径)
	float targetDensity = 120000.0f;  // 目標密度(この値に近づくように反発する)
	float pressureMultiplier = 0.1f;  // 押し出す力の強さ

	// 各粒子の「現在の密度」を計算する
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		float density = 0.0f;

		// 自分を含む、周りの全ての粒子との距離を測る
		for (int j = 0; j < kMaxParticles; j++)
		{
			if (particles_[j] == nullptr || !particles_[j]->isActive) continue;

			float dx = particles_[i]->predictedPosition.x - particles_[j]->predictedPosition.x;
			float dy = particles_[i]->predictedPosition.y - particles_[j]->predictedPosition.y;
			float distSq = dx * dx + dy * dy;

			// smoothingRadiusの範囲内にいる粒子だけを計算
			if (distSq < smoothingRadius * smoothingRadius)
			{
				float dist = sqrtf(distSq);
				float influence = smoothingRadius - dist; // 近いほど値が大きくなる

				// 影響度を3乗して密度に加算(中心に近いほど急激に密度が上がる)
				density += influence * influence * influence;
			}
		}
		// 自分の密度として保存
		particles_[i]->density = density;
	}

	// 密度を元に「圧力」を計算し、予測位置を押し戻す
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		Vector2 pushVelocity = { 0.0f, 0.0f };

		// 自分の圧力 = (現在の密度 - 目標密度) * 圧力係数
		// 密度が目標より高ければプラス(反発)、低ければマイナス(引き合う)になる
		float pressureI = (particles_[i]->density - targetDensity) * pressureMultiplier;

		for (int j = 0; j < kMaxParticles; j++)
		{
			if (i == j) continue; // 自分自身は弾かない
			if (particles_[j] == nullptr || !particles_[j]->isActive) continue;

			float dx = particles_[i]->predictedPosition.x - particles_[j]->predictedPosition.x;
			float dy = particles_[i]->predictedPosition.y - particles_[j]->predictedPosition.y;
			float distSq = dx * dx + dy * dy;

			if (distSq < smoothingRadius * smoothingRadius)
			{
				float dist = sqrtf(distSq);
				if (dist < 0.0001f) continue; // 完全に重なっている場合のゼロ除算を回避

				// 相手の圧力
				float pressureJ = (particles_[j]->density - targetDensity) * pressureMultiplier;

				// お互いの圧力を平均化する(作用・反作用の法則)
				float sharedPressure = (pressureI + pressureJ) * 0.5f;

				// マイナスの圧力(引力)をゼロにする
				if (sharedPressure < 0.0f)
				{
					sharedPressure = 0.0f;
				}

				// 押し出す強さの計算(近いほど強く押し出す)
				float influence = smoothingRadius - dist;
				float pushForce = sharedPressure * (influence * influence) / particles_[j]->density;

				// 距離ベクトルを正規化して、押し出し力を掛ける
				pushVelocity.x += (dx / dist) * pushForce;
				pushVelocity.y += (dy / dist) * pushForce;
			}
		}

		// 計算した反発力を使って、予測位置をずらす
		particles_[i]->predictedPosition.x += pushVelocity.x * deltaTime;
		particles_[i]->predictedPosition.y += pushVelocity.y * deltaTime;
	}


	// 速度の再計算と位置の確定
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		// 修正された予測位置から、実際の速度を逆算する
		particles_[i]->velocity.x = (particles_[i]->predictedPosition.x - particles_[i]->position.x) / deltaTime;
		particles_[i]->velocity.y = (particles_[i]->predictedPosition.y - particles_[i]->position.y) / deltaTime;

		// 速度を少し減衰させる
		particles_[i]->velocity.x *= 0.98f;
		particles_[i]->velocity.y *= 0.98f;

		// 位置を確定させる
		particles_[i]->position = particles_[i]->predictedPosition;

		// 経過時間を更新
		particles_[i]->elapsedTime += deltaTime;
	}

}

void ParticleSystem::Draw()
{

	// コップの枠線を描画
	Novice::DrawBox(540, 300, 200, 300, 0.0f, 0x555555FF, kFillModeWireFrame);

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


