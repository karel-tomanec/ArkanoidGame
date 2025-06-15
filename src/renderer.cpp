#include "renderer.hpp"

#include <filesystem>

Renderer::Renderer(SDL_Window* window, const Vector2& logicalSize, const Vector2& screenSize)
{
	if (!TTF_Init())
	{
		throw std::runtime_error("TTF_Init failed.");
	}

	mRenderer = SDL_CreateRenderer(window, nullptr);
	if (!mRenderer)
	{
		throw std::runtime_error(std::format("SDL_CreateRenderer Error: {}", SDL_GetError()));
	}

	loadFont();

	setLogicalResolution(logicalSize, screenSize);
}

Renderer::~Renderer()
{
	if (mFont)
	{
		TTF_CloseFont(mFont);
		mFont = nullptr;
	}

	if (mRenderer)
	{
		SDL_DestroyRenderer(mRenderer);
		mRenderer = nullptr;
	}

	TTF_Quit();
}

void Renderer::clearScreen() const
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);
}

void Renderer::presentFrame() const
{
	SDL_RenderPresent(mRenderer);
}

void Renderer::drawFilledCircle(const Vector2& position, float radius, const SDL_Color& color) const
{
	constexpr int segments = 32;
	std::vector<SDL_Vertex> vertices(segments + 1);
	std::vector<int> indices;

	const SDL_FColor fColor = toFColor(color);

	// Center vertex
	auto pos = toScreen(position);
	vertices[0].position = { .x = pos.x, .y = pos.y };
	vertices[0].color = fColor;

	// Outer vertices
	for (int i = 0; i < segments; ++i)
	{
		float angle = 2.0f * pi * static_cast<float>(i) / static_cast<float>(segments);
		float x = std::cos(angle) * radius * mScale.x;
		float y = std::sin(angle) * radius * mScale.y;

		vertices[i + 1].position = { .x = pos.x + x, .y = pos.y + y };
		vertices[i + 1].color = fColor;
	}

	for (int i = 0; i < segments; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back((i + 1) % segments + 1);
	}

	SDL_RenderGeometry(mRenderer, nullptr, vertices.data(), static_cast<int>(vertices.size()),
		indices.data(), static_cast<int>(indices.size()));
}

void Renderer::drawRectangle(const Vector2& position, const Vector2& size, const SDL_Color& color) const
{
	setDrawColor(color);
	Vector2 screenPos = toScreen(position);
	Vector2 screenSize = size * mScale;

	SDL_FRect rect{ screenPos.x - screenSize.x * 0.5f, screenPos.y - screenSize.y * 0.5f, screenSize.x, screenSize.y };
	SDL_RenderRect(mRenderer, &rect);
}

void Renderer::drawFilledRectangle(const Vector2& position, const Vector2& size, const SDL_Color& color) const
{
	setDrawColor(color);
	Vector2 screenPos = toScreen(position);
	Vector2 screenSize = size * mScale;

	SDL_FRect rect{ screenPos.x - screenSize.x * 0.5f, screenPos.y - screenSize.y * 0.5f, screenSize.x, screenSize.y };
	SDL_RenderFillRect(mRenderer, &rect);
}

void Renderer::setDrawColor(const SDL_Color& color) const
{
	SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
}

SDL_FColor Renderer::toFColor(const SDL_Color& c)
{
	return SDL_FColor{
		c.r / 255.0f,
		c.g / 255.0f,
		c.b / 255.0f,
		c.a / 255.0f
	};
}

void Renderer::drawText(const std::string& text, const Vector2& position, const SDL_Color& color, TextAlign align) const
{
	if (!mFont || text.empty()) return;

	SDL_Surface* surface = TTF_RenderText_Blended(mFont, text.c_str(), text.size(), color);
	if (!surface) return;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
	SDL_DestroySurface(surface);
	if (!texture) return;

	float w, h;
	SDL_GetTextureSize(texture, &w, &h);
	Vector2 screenPos = toScreen(position);
	SDL_FRect dst = { screenPos.x, screenPos.y, w, h };

	// Adjust based on alignment
	switch (align)
	{
		case TextAlign::TopCenter:
		case TextAlign::MiddleCenter:
		case TextAlign::BottomCenter:
			dst.x -= w / 2.0f;
			break;

		case TextAlign::TopRight:
		case TextAlign::MiddleRight:
		case TextAlign::BottomRight:
			dst.x -= w;
			break;

		default: break;
	}

	switch (align)
	{
		case TextAlign::MiddleLeft:
		case TextAlign::MiddleCenter:
		case TextAlign::MiddleRight:
			dst.y -= h / 2.0f;
			break;

		case TextAlign::BottomLeft:
		case TextAlign::BottomCenter:
		case TextAlign::BottomRight:
			dst.y -= h;
			break;

		default: break;
	}

	SDL_RenderTexture(mRenderer, texture, nullptr, &dst);
	SDL_DestroyTexture(texture);
}

void Renderer::loadFont()
{
	mFont = TTF_OpenFont("assets/fonts/slkscr.ttf", mCurrentFontSize);
	if (!mFont)
	{
		throw std::runtime_error("Failed to load font.");
	}
}

void Renderer::setLogicalResolution(const Vector2& logicalSize, const Vector2& screenSize)
{
	mLogicalSize = logicalSize;
	mScreenSize = screenSize;
	mScale.x = screenSize.x / logicalSize.x;
	mScale.y = screenSize.y / logicalSize.y;

	// Compute desired font size (rounded)
	float newFontSize = std::round(mBaseFontSize * mScale.y);

	// Reload font if size changed
	if (newFontSize != mCurrentFontSize)
	{
		mCurrentFontSize = newFontSize;

		if (mFont)
		{
			TTF_CloseFont(mFont);
			mFont = nullptr;
		}
		loadFont();
	}
}

Vector2 Renderer::toScreen(const Vector2& logical) const
{
	return {
		logical.x * mScale.x,
		logical.y * mScale.y
	};
}