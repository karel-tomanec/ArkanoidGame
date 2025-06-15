#include "block.hpp"
#include "color.hpp"

Block::Block(const Vector2& position, const Vector2& size, BlockType type)
	: GameObject(position, getColorForType(type)), mSize(size), mType(type)
{
	if (type == BlockType::Reinforced)
	{
		mLifeCount = 3;
	}
}

void Block::render(const Renderer& renderer) const
{
	if (mIsDestroyed)
		return;

	SDL_Color renderColor = mColor;

	if (mType == BlockType::Reinforced)
	{
		// Assuming max life count is 3
		const float brightness = 0.4f + 0.2f * static_cast<float>(mLifeCount);
		renderColor.r = static_cast<Uint8>(renderColor.r * brightness);
		renderColor.g = static_cast<Uint8>(renderColor.g * brightness);
		renderColor.b = static_cast<Uint8>(renderColor.b * brightness);
	}

	renderer.drawFilledRectangle(mPosition, mSize, renderColor);
	renderer.drawRectangle(mPosition, mSize, Color::Black);
}

const Vector2& Block::getSize() const
{
	return mSize;
}

AABB Block::getAABB() const
{
	return { mPosition - 0.5f * mSize, mPosition + 0.5f * mSize };
}

BlockType Block::getType() const
{
	return mType;
}

bool Block::isDestroyed() const
{
	return mIsDestroyed;
}

void Block::tryDestroy()
{
	mLifeCount--;
	if (mLifeCount == 0)
	{
		mIsDestroyed = true;
	}
}

uint32_t Block::getScore() const
{
	switch (mType)
	{
		case BlockType::Normal: return 1;
		case BlockType::Booster: return 2;
		case BlockType::Reinforced: return 3;
		default: return 0; // Fallback
	}
}

SDL_Color Block::getColorForType(BlockType type)
{
	switch (type)
	{
		case BlockType::Normal: return Color::Cyan;
		case BlockType::Booster: return Color::Magenta;
		case BlockType::Reinforced: return Color::Blue;
		default: return Color::White; // Fallback color
	}
}
