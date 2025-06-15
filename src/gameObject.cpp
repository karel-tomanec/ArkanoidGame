#include "gameObject.hpp"

GameObject::GameObject(Vector2 position, SDL_Color color) : mPosition(position), mColor(color)
{
}

const Vector2& GameObject::getPosition() const
{
	return mPosition;
}

const SDL_Color& GameObject::getColor() const
{
	return mColor;
}

void GameObject::setPosition(const Vector2& position)
{
	mPosition = position;
}

void GameObject::setColor(const SDL_Color& color)
{
	mColor = color;
}
