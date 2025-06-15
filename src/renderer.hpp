#pragma once
#include <format>
#include <stdexcept>

#include "math.hpp"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

enum class TextAlign : std::uint8_t
{
	TopLeft,
	TopCenter,
	TopRight,
	MiddleLeft,
	MiddleCenter,
	MiddleRight,
	BottomLeft,
	BottomCenter,
	BottomRight
};

class Renderer final
{
public:
	Renderer(SDL_Window* window, const Vector2& logicalSize, const Vector2& screenSize);

	~Renderer();

	void clearScreen() const;

	void presentFrame() const;

	void drawFilledCircle(const Vector2& position, float radius, const SDL_Color& color) const;

	void drawRectangle(const Vector2& position, const Vector2& size, const SDL_Color& color) const;

	void drawFilledRectangle(const Vector2& position, const Vector2& size, const SDL_Color& color) const;

	void drawText(const std::string& text, const Vector2& position, const SDL_Color& color, TextAlign align = TextAlign::MiddleLeft) const;
	void loadFont();

	void setLogicalResolution(const Vector2& logicalSize, const Vector2& screenSize);

private:

	void setDrawColor(const SDL_Color& color) const;

	static SDL_FColor toFColor(const SDL_Color& c);

	Vector2 toScreen(const Vector2& logical) const;

	SDL_Renderer* mRenderer = nullptr;
	TTF_Font* mFont = nullptr;

	Vector2 mLogicalSize;
	Vector2 mScreenSize;
	Vector2 mScale;

	float mBaseFontSize = 24.f; // Design-time font size (works well at 800x800)
	float mCurrentFontSize = 24.f;
};