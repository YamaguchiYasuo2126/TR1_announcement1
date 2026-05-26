#pragma once
#include "Vector2.h"

const int kMaxParticles = 600;

typedef struct Particle
{
	Vector2 position{};
	Vector2 velocity{};
	Vector2 acceleration{};
	Vector2 initialVelocity{};
	float radius = 10.0f;
	float lifeTime = 0;
	int isActive = false;
	unsigned int color = 0xFFFFFFFF;
	float elapsedTime = 0.0f;
	int isBubble = false;
	int isFire = false;
	int isFog = false;
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
	void Emit(const Vector2& position, const Vector2& velocity, const Vector2& acceleration, float lifeTime, float radius, unsigned int color, int isBubble, int isFire, int isFog);

private:
	Particle* particles_[kMaxParticles]{};

};