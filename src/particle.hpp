#pragma once
#include "dynamicGameObject.hpp"

class Particle final : public DynamicGameObject
{
public:

	Particle(const Vector2& position, const Vector2& size, const SDL_Color& color, float speed, const Vector2& direction, float lifetime);

	void render(const Renderer& renderer) const override;

	AABB getAABB() const override;

	const Vector2& getSize() const;

	float getLifetime() const;

	void setSize(const Vector2& size);

	void setLifetime(float lifetime);

private:
	Vector2 mSize;
	float mLifetime;
};