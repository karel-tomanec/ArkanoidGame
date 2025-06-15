#include "particle.hpp"

Particle::Particle(const Vector2& position, const Vector2& size, const SDL_Color& color, float speed, const Vector2& direction, float lifetime)
	: DynamicGameObject(position, color), mSize(size)
{
	mSpeed = speed;
	mDirection = normalize(direction);
	mLifetime = lifetime;
}

void Particle::render(const Renderer& renderer) const
{
	renderer.drawFilledRectangle(mPosition, mSize, mColor);
}

AABB Particle::getAABB() const
{
	return { .min = mPosition - 0.5f * mSize, .max = mPosition + 0.5f * mSize };
}

const Vector2& Particle::getSize() const
{
	return mSize;
}

float Particle::getLifetime() const
{
	return mLifetime;
}

void Particle::setSize(const Vector2& size)
{
	mSize = size;
}

void Particle::setLifetime(float lifetime)
{
	mLifetime = lifetime;
}
