#pragma once

#include "gameObject.hpp"

class DynamicGameObject : public GameObject
{
public:

	DynamicGameObject(const Vector2& position, const SDL_Color& color);

	~DynamicGameObject() override = default;

	const Vector2& getDirection() const;

	float getSpeed() const;

	void setDirection(const Vector2& direction);

	void setSpeed(float speed);

protected:

	Vector2 mDirection;
	float mSpeed;
};
