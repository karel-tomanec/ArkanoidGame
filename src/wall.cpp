#include "Wall.hpp"
#include "color.hpp"

Wall::Wall(const Vector2& position, const Vector2& size, const SDL_Color& color)
	: GameObject(position, color), mSize(size)
{
}

void Wall::render(const Renderer& renderer) const
{
	renderer.drawFilledRectangle(mPosition, mSize, mColor);
}

Vector2 Wall::getSize() const
{
	return mSize;
}

AABB Wall::getAABB() const
{
	return { mPosition - 0.5f * mSize, mPosition + 0.5f * mSize };
}
