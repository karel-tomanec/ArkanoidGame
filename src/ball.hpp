#pragma once
#include "color.hpp"
#include "dynamicGameObject.hpp"
#include "SDL3/SDL.h"

class Ball final : public DynamicGameObject
{
public:
	Ball(const Vector2& position, float radius, float defaultSpeed);

	void render(const Renderer& renderer) const override;

	AABB getAABB() const override;

	float getRadius() const;

	void resetSpeedAndColor();

private:

	float mRadius;
	float defaultSpeed;

	static constexpr SDL_Color kDefaultColor = Color::White;;
};