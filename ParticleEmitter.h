#pragma once
#include "Vector2.h"
#include "AABB.h"
#include <stdlib.h>

class ParticleSystem;

class ParticleEmitter
{
public:

	ParticleEmitter() = delete;
	ParticleEmitter(ParticleSystem* particleSystem)
		:particleSystem_(particleSystem)
	{
		emittArea_.max.x = 540;
		emittArea_.max.y = 260;
		emittArea_.min.x = 740;
		emittArea_.min.y = 460;
		emittCenterPosition_.x = 640.0f;
		emittCenterPosition_.y = 360.0f;
	};

	void Emit();

	void Draw();
	void SetPosition(const Vector2& position);


public:
	// 固定の初速度
	Vector2 velocity_{ 0.0f, 0.0f };
	// 固定の加速度
	Vector2 acceleration_{ 0.0f, 0.0f };
	// 生存時間
	float lifeTime_ = 2.0f;
	// 半径
	float radius_ = 10.0f;

	// 色
	unsigned int color_ = 0xFFFFFFFF;


	Vector2 emittSize_{ 100.0f, 100.0f };

	int isBubble_ = false;

	int isFire_ = false;

	int isFog_ = false;

	// シーン設定を一括変更する関数
	void SetEmitterConfig(const Vector2& velocity, const Vector2& acceleration, float lifeTime, unsigned int color, const Vector2& emittSize, float minRadius, float maxRadius);

	float minRadius_ = 10.0f;
	float maxRadius_ = 10.0f;

private:
	ParticleSystem* particleSystem_ = nullptr;
	AABBInt emittArea_{};
	Vector2 emittCenterPosition_;

};

