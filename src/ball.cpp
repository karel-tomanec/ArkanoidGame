#include "ball.hpp"

#include "math.hpp"

Ball::Ball(const Vector2& position, float radius, float defaultSpeed)
	: DynamicGameObject(position, kDefaultColor), mRadius(radius), defaultSpeed(defaultSpeed)
{
	mSpeed = defaultSpeed;
}

void Ball::render(const Renderer& renderer) const
{
	renderer.drawFilledCircle(mPosition, mRadius, mColor);
}

AABB Ball::getAABB() const
{
	return { .min = mPosition - Vector2{ mRadius, mRadius }, .max = mPosition + Vector2{ mRadius, mRadius } };
}

float Ball::getRadius() const
{
	return mRadius;
}

void Ball::resetSpeedAndColor()
{
	mSpeed = defaultSpeed;
	mColor = kDefaultColor;
}
