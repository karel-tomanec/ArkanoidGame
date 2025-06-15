#include "particleSystem.hpp"
#include "math.hpp"
#include "gameConfig.hpp"

ParticleSystem::ParticleSystem(std::mt19937& rng)
	: mRng(rng)
{
}

void ParticleSystem::update(double deltaTime)
{
	for (auto& p : mParticles)
	{
		p.setLifetime(p.getLifetime() - static_cast<float>(deltaTime));
		if (p.getLifetime() > 0.f)
		{
			p.setPosition(p.getPosition() + p.getDirection() * p.getSpeed() * static_cast<float>(deltaTime));
			p.setSize(p.getSize() * (1.f - static_cast<float>(deltaTime)));
		}
	}

	mTempBuffer.clear();
	for (const auto& p : mParticles)
	{
		if (p.getLifetime() > 0.f)
			mTempBuffer.push_back(p);
	}
	std::swap(mParticles, mTempBuffer);
}

void ParticleSystem::render(const Renderer& renderer) const
{
	for (const auto& p : mParticles)
		p.render(renderer);
}

void ParticleSystem::emitFromBlock(const Block& block)
{
	std::uniform_real_distribution rnd(0.f, 1.f);
	for (uint32_t i = 0; i < GameConfig::kParticlesPerBlock; ++i)
	{
		float lifetime = 1.f + rnd(mRng) * 2.f;
		Vector2 pos = block.getPosition() + randomPointInRectangle(block.getSize(), { rnd(mRng), rnd(mRng) });
		Vector2 dir = { 0.f, 1.f };
		Vector2 size = Vector2{ 2.f } + rnd(mRng) * Vector2 { 4.f };
		float speed = 100.f + rnd(mRng) * 100.f;
		mParticles.emplace_back(pos, size, block.getColor(), speed, dir, lifetime);
	}
}

void ParticleSystem::emitFromBall(const Ball& ball)
{
	std::uniform_real_distribution rnd(0.f, 1.f);
	float lifetime = 0.5f + rnd(mRng) * 0.5f;
	Vector2 pos = ball.getPosition() + randomPointInCircle(ball.getRadius(), { rnd(mRng), rnd(mRng) });
	mParticles.emplace_back(pos, Vector2{ 4.f }, ball.getColor(), ball.getSpeed() * 0.1f, ball.getDirection(), lifetime);
}

void ParticleSystem::emitFromPlatform(const Platform& platform)
{
	if (lengthSquared(platform.getDirection()) == 0.f)
		return;

	std::uniform_real_distribution rnd(0.f, 1.f);
	float lifetime = 0.1f + rnd(mRng) * 0.5f;
	Vector2 pos = platform.getPosition() + randomPointInRectangle(platform.getSize(), { rnd(mRng), rnd(mRng) });
	float speed = platform.getSpeed() * 0.1f;
	mParticles.emplace_back(pos, Vector2{ 2.f }, platform.getColor(), speed, platform.getDirection(), lifetime);
}
