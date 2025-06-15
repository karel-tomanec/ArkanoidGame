#pragma once
#include "gameObject.hpp"

class Wall final : public GameObject
{
public:
	Wall(const Vector2& position, const Vector2& size, const SDL_Color& color);

	void render(const Renderer& renderer) const override;

	Vector2 getSize() const;

	AABB getAABB() const override;

private:
	Vector2 mSize;
};