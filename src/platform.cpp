#include "platform.hpp"

#include "math.hpp"

Platform::Platform(const Vector2& position, const Vector2& size, const SDL_Color& color)
	: DynamicGameObject(position, color), mSize(size)
{
	mSpeed = 500.f;
}

void Platform::render(const Renderer& renderer) const
{
	renderer.drawFilledRectangle(mPosition, mSize, mColor);
}

void Platform::handleInput(const MoveDirection& moveDirection)
{
	switch (moveDirection)
	{
		case MoveDirection::Left:
			mDirection = Vector2{ -mSpeed, 0.f };
			break;
		case MoveDirection::Right:
			mDirection = Vector2{ mSpeed, 0.f };
			break;
		default:
			mDirection = {};
			break;
	}
}

AABB Platform::getAABB() const
{
	return { .min = mPosition - 0.5f * mSize, .max = mPosition + 0.5f * mSize };
}

const Vector2& Platform::getSize() const
{
	return mSize;
}
