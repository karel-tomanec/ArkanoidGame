#pragma once
#include "dynamicGameObject.hpp"
#include "SDL3/SDL.h"

#include "gameObject.hpp"

enum class MoveDirection : std::uint8_t
{
	None,
	Left,
	Right
};

class Platform final : public DynamicGameObject
{
public:
	Platform(const Vector2& position, const Vector2& size, const SDL_Color& color);

	void render(const Renderer& renderer) const override;

	void handleInput(const MoveDirection& moveDirection);

	AABB getAABB() const override;

	const Vector2& getSize() const;

private:
	Vector2 mSize;
};