#pragma once

#include <vector>
#include <random>
#include "particle.hpp"
#include "block.hpp"
#include "ball.hpp"
#include "platform.hpp"
#include "renderer.hpp"

class ParticleSystem
{
public:
	ParticleSystem(std::mt19937& rng);

	void update(double deltaTime);

	void render(const Renderer& renderer) const;

	void emitFromBlock(const Block& block);

	void emitFromBall(const Ball& ball);

	void emitFromPlatform(const Platform& platform);

private:
	std::vector<Particle> mParticles;
	std::vector<Particle> mTempBuffer;
	std::mt19937& mRng;
};
