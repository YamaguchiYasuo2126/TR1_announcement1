#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "NoviceDrawUtility.h"
#include <stdlib.h>

void ParticleEmitter::Emit()
{
	Vector2Int randomArea =
	{
		.x = emittArea_.max.x - emittArea_.min.x,
		.y = emittArea_.max.y - emittArea_.min.y,
	};

	randomArea.x = (randomArea.x > 0) ? randomArea.x : 1;
	randomArea.y = (randomArea.y > 0) ? randomArea.y : 1;

	// emittArea_.minを基点とする
	Vector2 randomOffset =
	{
		.x = static_cast<float>(rand() % randomArea.x),
		.y = static_cast<float>(rand() % randomArea.y),
	};

	Vector2 position =
	{
		.x = static_cast<float>(emittArea_.min.x) + randomOffset.x,
		.y = static_cast<float>(emittArea_.min.y) + randomOffset.y,
	};

	// 速度、加速度、その他のパラメータはメンバー変数の固定値を使用

	Vector2 velocity = velocity_;
	Vector2 acceleration = acceleration_;
	float lifeTime = lifeTime_;

	// エミットのたびに半径をランダムに設定する
	float radius = minRadius_;
	if (maxRadius_ > minRadius_)
	{
		// ランダムなサイズを計算
		radius = minRadius_ + static_cast<float>(rand() % static_cast<int>(maxRadius_ - minRadius_));
	}

	unsigned int color = color_;

	if (particleSystem_ != nullptr)
	{

		particleSystem_->Emit(position, velocity, acceleration, lifeTime, radius, color, isBubble_, isFire_, isFog_);
	}

}

void ParticleEmitter::SetPosition(const Vector2& position)
{
	// 中心位置を更新
	emittCenterPosition_ = position;

	// 幅と高さが負の値にならないよう、絶対値を使うか、常に正の値であることを前提とする
	float safeWidth = (emittSize_.x > 0.0f) ? emittSize_.x : 1.0f;
	float safeHeight = (emittSize_.y > 0.0f) ? emittSize_.y : 1.0f;

	// エリアの半分のサイズを現在の幅/高さから計算
	int halfWidth = static_cast<int>(safeWidth / 2.0f);
	int halfHeight = static_cast<int>(safeHeight / 2.0f);

	// 中心位置から半分のサイズを引いた/足した座標で矩形範囲を更新
	emittArea_.min.x = static_cast<int>(position.x) - halfWidth;
	emittArea_.min.y = static_cast<int>(position.y) - halfHeight;
	emittArea_.max.x = static_cast<int>(position.x) + halfWidth;
	emittArea_.max.y = static_cast<int>(position.y) + halfHeight;
}

void ParticleEmitter::SetEmitterConfig(const Vector2& velocity, const Vector2& acceleration, float lifeTime, unsigned int color, const Vector2& emittSize, float minRadius, float maxRadius)
{
	velocity_ = velocity;
	acceleration_ = acceleration;
	lifeTime_ = lifeTime;
	color_ = color;
	emittSize_ = emittSize;
	minRadius_ = minRadius;
	maxRadius_ = maxRadius;
}

void ParticleEmitter::Draw()
{
	// エミットエリアを黄色の枠線で描画
	unsigned int color = 0xFFFFFFFF;

	DrawBox(emittArea_, color);
}