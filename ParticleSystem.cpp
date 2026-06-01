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

	// コップ（境界）のパラメータ
	float cupLeft = 540.0f;
	float cupRight = 740.0f;
	float cupBottom = 600.0f;

	// 外力の適用と、予測位置の計算
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		// 重力（加速度）を速度に加算
		particles_[i]->velocity.x += particles_[i]->acceleration.x * deltaTime;
		particles_[i]->velocity.y += particles_[i]->acceleration.y * deltaTime;

		// 予測位置(predictedPosition)を計算
		particles_[i]->predictedPosition.x = particles_[i]->position.x + particles_[i]->velocity.x * deltaTime;
		particles_[i]->predictedPosition.y = particles_[i]->position.y + particles_[i]->velocity.y * deltaTime;

		// 境界（コップ）との当たり判定
		// 底にぶつかったら、底の高さに押し戻す
		if (particles_[i]->predictedPosition.y > cupBottom - particles_[i]->radius)
		{
			particles_[i]->predictedPosition.y = cupBottom - particles_[i]->radius;
		}
		// 左壁にぶつかったら押し戻す
		if (particles_[i]->predictedPosition.x < cupLeft + particles_[i]->radius)
		{
			particles_[i]->predictedPosition.x = cupLeft + particles_[i]->radius;
		}
		// 右壁にぶつかったら押し戻す
		if (particles_[i]->predictedPosition.x > cupRight - particles_[i]->radius)
		{
			particles_[i]->predictedPosition.x = cupRight - particles_[i]->radius;
		}
	}

	// 粒子同士の衝突解決

	// 速度の再計算と位置の確定
	for (int i = 0; i < kMaxParticles; i++)
	{
		if (particles_[i] == nullptr || !particles_[i]->isActive) continue;

		// 修正された予測位置から、実際の速度を逆算する
		particles_[i]->velocity.x = (particles_[i]->predictedPosition.x - particles_[i]->position.x) / deltaTime;
		particles_[i]->velocity.y = (particles_[i]->predictedPosition.y - particles_[i]->position.y) / deltaTime;

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


