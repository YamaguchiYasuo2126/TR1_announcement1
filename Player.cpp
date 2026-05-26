#include "Player.h"
#include "Novice.h"
#include "NoviceDrawUtility.h"

void Player::Initialize(ParticleSystem* particleSystem)
{
	pos_ = { 200.0f, 360.0f };
	speed_ = 6.0f;
	size_ = { 64.0f, 64.0f };

	// プレイヤー用のエミッターを生成
	// メインのパーティクルシステムを共有して使う
	trailEmitter_ = new ParticleEmitter(particleSystem);

	// 軌跡用パーティクルの設定
	trailEmitter_->SetEmitterConfig({ 0.0f, 0.0f }, { 0.0f, 0.0f }, 0.5f, 0xFFC9D2FF, size_, 10.0f, 14.0f);
}

void Player::Update(char* keys)
{
	// WASD移動
	if (keys[DIK_W] || keys[DIK_UP])
	{
		pos_.y -= speed_;
	}

	if (keys[DIK_S] || keys[DIK_DOWN])
	{
		pos_.y += speed_;

	}

	if (keys[DIK_A] || keys[DIK_LEFT])
	{
		pos_.x -= speed_;
	}

	if (keys[DIK_D] || keys[DIK_RIGHT])
	{
		pos_.x += speed_;
	}

	if (pos_.x >= 1280.0f - size_.x / 2.0f)
	{
		pos_.x = 1280.0f - size_.x / 2.0f;
	}

	if (pos_.x <= 0.0f + size_.x / 2.0f)
	{
		pos_.x = 0.0f + size_.x / 2.0f;
	}

	if (pos_.y >= 720.0f - size_.y / 2.0f)
	{
		pos_.y = 720.0f - size_.y / 2.0f;
	}

	if (pos_.y <= 0.0f + size_.y / 2.0f)
	{
		pos_.y = 0.0f + size_.y / 2.0f;
	}

	// エミッターの位置をプレイヤーに合わせる
	if (trailEmitter_)
	{
		// プレイヤーの中心にエミッターをセット
		trailEmitter_->SetPosition(pos_);

		// 常にパーティクルを出し続ける
		trailEmitter_->Emit();
	}
}

// プレイヤー本体の描画
void Player::Draw()
{

	Novice::DrawBox
	(
		static_cast<int>(pos_.x - size_.x / 2.0f),
		static_cast<int>(pos_.y - size_.y / 2.0f),
		static_cast<int>(size_.x), static_cast<int>(size_.y),
		0.0f, 0xFFC9D2FF, kFillModeSolid
	);

}
