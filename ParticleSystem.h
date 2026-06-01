#pragma once
#include "Vector2.h"


const int kMaxParticles = 600;

typedef struct Particle
{
	Vector2 position{};
	Vector2 velocity{};
	Vector2 acceleration{};
	Vector2 initialVelocity{};

	Vector2 predictedPosition{}; // 予測位置
	float density = 0.0f;        // 現在の密度（周りにどれくらい粒子がいるか）
	float lambda = 0.0f;         // 圧力の補正値

	float radius = 10.0f;
	int isActive = false;
	unsigned int color = 0xFFFFFFFF;
	float elapsedTime = 0.0f;
	int isBubble = false;
} Particle;


class ParticleSystem
{

public:


public:
	void Initialize();
	void Update();
	void Draw();
	void Finalize();
	void Reset();
	void Emit(const Vector2& position, const Vector2& velocity, const Vector2& acceleration, float radius, unsigned int color, int isBubble);

private:
	Particle* particles_[kMaxParticles]{};

};