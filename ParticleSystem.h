#pragma once
#include "Vector2.h"
#include <vector>

const int kMaxParticles = 600;

// 近傍探索(グリッド)用の設定
// セルのサイズは、PBFのsmoothingRadius(影響半径)と同じ値
const float kCellSize = 40.0f;
// 画面サイズ(1280x720)をセルサイズで割ったマス目の数
const int kGridWidth = 40;
const int kGridHeight = 25;

typedef struct Particle
{
	Vector2 position{};
	Vector2 velocity{};
	Vector2 acceleration{};
	Vector2 initialVelocity{};

	// PBF用に追加するパラメータ
	Vector2 predictedPosition{}; // 予測位置
	float density = 0.0f;        // 現在の密度(周りにどれくらい粒子がいるか)
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
	// 画面のマス目(各マスの中に、粒子のインデックス番号を保存する)
	std::vector<int> grid_[kGridWidth * kGridHeight];

};