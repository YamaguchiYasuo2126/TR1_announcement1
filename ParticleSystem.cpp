#include "ParticleSystem.h"
#include "NoviceDrawUtility.h"
#define _USE_MATH_DEFINES
#include<math.h>

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

		// 経過時間を更新
		particles_[i]->elapsedTime += deltaTime;

		particles_[i]->velocity.x += particles_[i]->acceleration.x;
		particles_[i]->velocity.y += particles_[i]->acceleration.y;

		particles_[i]->position.x += particles_[i]->velocity.x * deltaTime;
		particles_[i]->position.y += particles_[i]->velocity.y * deltaTime;

		// 霧の揺れとその他の揺れで分ける
		if (particles_[i]->isFog)
		{

			particles_[i]->position.x += sinf(particles_[i]->elapsedTime * 0.5f) * 0.2f;
			particles_[i]->position.y += cosf(particles_[i]->elapsedTime * 0.3f) * 0.2f;
		}
		else if (particles_[i]->isBubble)
		{
			particles_[i]->position.x += sinf(particles_[i]->elapsedTime * 10.0f) * 1.6f;
		}

		// 生存時間更新
		particles_[i]->lifeTime -= deltaTime;

		// 非アクティブ化
		if (particles_[i]->lifeTime < 0.0f)
		{
			particles_[i]->isActive = false;
		}
	}
}

void ParticleSystem::Draw()
{
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

		// パーティクルの全寿命を計算
		float totalLife = particles_[i]->lifeTime + particles_[i]->elapsedTime;
		float alphaRate = 1.0f;

		if (particles_[i]->isFog)
		{
			// フェードインフェードアウトのような処理
			if (particles_[i]->elapsedTime < 1.0f)
			{
				alphaRate = particles_[i]->elapsedTime / 1.0f;
			}
			else if (particles_[i]->lifeTime < 1.0f)
			{
				alphaRate = particles_[i]->lifeTime / 1.0f;
			}
		}
		else
		{
			alphaRate = particles_[i]->lifeTime / totalLife; // シンプルな減衰
		}

		if (alphaRate > 1.0f)
		{
			alphaRate = 1.0f;
		}

		if (alphaRate < 0.0f)
		{
			alphaRate = 0.0f;
		}

		// 元の色に設定されているアルファ値(濃さの最大値)を取り出す
		unsigned int maxAlpha = particles_[i]->color & 0xFF;

		// 最大値に対して、フェード率を掛ける
		unsigned int currentAlpha = static_cast<int>(alphaRate * maxAlpha);

		// RGB成分と、計算した新しいアルファ値を結合
		unsigned int drawColor = (particles_[i]->color & 0xFFFFFF00) | currentAlpha;

		// 炎の周りを明るくする
		if (particles_[i]->isFire)
		{

			float glowRadius = particles_[i]->radius * 2.0f;

			unsigned int glowAlpha = currentAlpha / 4;

			unsigned int glowColor = (particles_[i]->color & 0xFFFFFF00) | glowAlpha;

			DrawCircle(particles_[i]->position, glowRadius, glowColor);
		}

		DrawCircle(particles_[i]->position, particles_[i]->radius, drawColor);

		// 泡感を出すために白色の小さい円を描画
		if (particles_[i]->isBubble)
		{
			Vector2 highlightPos =
			{
				particles_[i]->position.x - particles_[i]->radius * 0.3f,
				particles_[i]->position.y - particles_[i]->radius * 0.3f
			};

			unsigned int whiteAlpha = static_cast<int>(alphaRate * 200.0f); // 少し控えめな白
			unsigned int highlightColor = (0xFFFFFF00) | whiteAlpha;

			DrawCircle(highlightPos, particles_[i]->radius * 0.4f, highlightColor);
		}
	}

}

void ParticleSystem::Finalize()
{
}

void ParticleSystem::Emit(const Vector2& position, const Vector2& velocity, const Vector2& acceleration, float lifeTime, float radius, unsigned int color, int isBubble, int isFire, int isFog)
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
		particles_[i]->lifeTime = lifeTime;
		particles_[i]->radius = radius;
		particles_[i]->color = color;
		particles_[i]->elapsedTime = 0.0f;
		particles_[i]->isBubble = isBubble;
		particles_[i]->isFire = isFire;
		particles_[i]->isFog = isFog;

		break;
	}
}


