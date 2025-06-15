#pragma once
#include "color.hpp"
#include "gameObject.hpp"

enum class BlockType
{
	Normal,
	Booster,
	Reinforced,
};

class Block final : public GameObject
{
public:
	Block(const Vector2& position, const Vector2& size, BlockType type);

	void render(const Renderer& renderer) const override;

	const Vector2& getSize() const;

	AABB getAABB() const override;

	BlockType getType() const;

	bool isDestroyed() const;

	void tryDestroy();

	uint32_t getScore() const;

private:

	static SDL_Color getColorForType(BlockType type);

	Vector2 mSize;
	BlockType mType = BlockType::Normal;
	uint8_t mLifeCount = 1;
	bool mIsDestroyed = false;
};