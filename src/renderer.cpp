#include "renderer.hpp"

#include <iostream>
#include <filesystem>

Renderer::Renderer(SDL_Window* window)
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

	mFont = TTF_OpenFont("assets/fonts/slkscr.ttf", 24);
	if (!mFont)
	{
		throw std::runtime_error("Failed to load font.");
	}
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
	vertices[0].position = { .x = position.x, .y = position.y };
	vertices[0].color = fColor;

	// Outer vertices
	for (int i = 0; i < segments; ++i)
	{
		float angle = 2.0f * pi * static_cast<float>(i) / static_cast<float>(segments);
		float x = std::cos(angle) * radius;
		float y = std::sin(angle) * radius;

		vertices[i + 1].position = { .x = position.x + x, .y = position.y + y };
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
	SDL_FRect rect{ position.x - size.x * 0.5f, position.y - size.y * 0.5f, size.x, size.y };
	SDL_RenderRect(mRenderer, &rect);
}

void Renderer::drawFilledRectangle(const Vector2& position, const Vector2& size, const SDL_Color& color) const
{
	setDrawColor(color);
	SDL_FRect rect{ position.x - size.x * 0.5f, position.y - size.y * 0.5f, size.x, size.y };
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

	SDL_FRect dst = { position.x, position.y, w, h };

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


