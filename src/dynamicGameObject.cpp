#include "dynamicGameObject.hpp"

DynamicGameObject::DynamicGameObject(const Vector2& position, const SDL_Color& color)
	: GameObject(position, color), mDirection(0.f, 0.f), mSpeed(0.f)
{
}

const Vector2& DynamicGameObject::getDirection() const
{
	return mDirection;
}

float DynamicGameObject::getSpeed() const
{
	return mSpeed;
}

void DynamicGameObject::setDirection(const Vector2& direction)
{
	mDirection = normalize(direction);
}

void DynamicGameObject::setSpeed(float speed)
{
	mSpeed = speed;
}
