#pragma once
#include "Vector2.h"
#include "ParticleEmitter.h"

class Player
{
public:
	// 初期化
	void Initialize(ParticleSystem* particleSystem);

	// 更新
	void Update(char* keys);

	// 描画
	void Draw();

private:
	Vector2 pos_{};
	float speed_{};
	Vector2 size_{};

	// プレイヤー専用のエミッター
	ParticleEmitter* trailEmitter_ = nullptr;
};

