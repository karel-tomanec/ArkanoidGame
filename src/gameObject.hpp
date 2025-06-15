#pragma once

#include "math.hpp"
#include "renderer.hpp"

class GameObject
{
public:
	GameObject(Vector2 position, SDL_Color color);

	virtual ~GameObject() = default;

	virtual void render(const Renderer& renderer) const = 0;

	virtual AABB getAABB() const = 0;

	const Vector2& getPosition() const;

	const SDL_Color& getColor() const;

	void setPosition(const Vector2& position);

	void setColor(const SDL_Color& color);

protected:
	Vector2 mPosition;
	SDL_Color mColor;
};
