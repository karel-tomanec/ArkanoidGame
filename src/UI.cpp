#include "ui.hpp"

#include <format>

#include "color.hpp"
#include "gameConfig.hpp"

namespace UIConfig
{
	inline constexpr Vector2 kScorePos = { 20.f, 750.f };
	inline constexpr Vector2 kBottomCenter = { 400.f, 750.f };
	inline constexpr Vector2 kCenter = { 400.f, 350.f };
	inline constexpr float kLifeIconSpacing = 30.f;
	inline constexpr float kLifeIconBaseX = 710.f;
	inline constexpr float kLifeIconY = 740.f;
}

UI::UI(Renderer& renderer)
	: mRenderer(renderer)
{
}

void UI::render(GameState gameState, uint32_t score, uint32_t lifeCount, bool hasMoved) const
{
	switch (gameState)
	{
		case GameState::Running:        renderRunning(score, lifeCount, hasMoved); break;
		case GameState::Paused:         renderPaused(score, lifeCount); break;
		case GameState::AwaitingServe:  renderAwaitingServe(score, lifeCount); break;
		case GameState::GameOver:       renderGameOver(score); break;
		case GameState::Won:            renderWon(score, lifeCount); break;
		case GameState::NotStarted:     renderNotStarted(); break;
	}
}

void UI::renderRunning(uint32_t score, uint32_t lifeCount, bool hasMoved) const
{
	drawScore(score);
	drawLives(lifeCount, GameState::Running);

	if (!hasMoved)
	{
		mRenderer.drawText("Use LEFT and RIGHT arrows to move", UIConfig::kCenter, Color::White, TextAlign::MiddleCenter);
	}
}

void UI::renderPaused(uint32_t score, uint32_t lifeCount) const
{
	drawScore(score);
	drawLives(lifeCount, GameState::Paused);
	mRenderer.drawText("Game Paused", UIConfig::kCenter, Color::Yellow, TextAlign::MiddleCenter);
	mRenderer.drawText("Press SPACE to resume", UIConfig::kBottomCenter, Color::White, TextAlign::MiddleCenter);
}

void UI::renderAwaitingServe(uint32_t score, uint32_t lifeCount) const
{
	drawScore(score);
	drawLives(lifeCount, GameState::AwaitingServe);
	mRenderer.drawText("Press SPACE to serve the ball", UIConfig::kCenter, Color::White, TextAlign::MiddleCenter);
}

void UI::renderGameOver(uint32_t score) const
{
	drawScore(score);
	mRenderer.drawText("Game Over", UIConfig::kCenter, Color::Red, TextAlign::MiddleCenter);
	mRenderer.drawText("Press SPACE to start again", UIConfig::kBottomCenter, Color::White, TextAlign::MiddleCenter);
}

void UI::renderWon(uint32_t score, uint32_t lifeCount) const
{
	drawScore(score);
	drawLives(lifeCount, GameState::Won);
	mRenderer.drawText("You won!", UIConfig::kCenter, Color::Green, TextAlign::MiddleCenter);
	mRenderer.drawText("Press SPACE to start again", UIConfig::kBottomCenter, Color::White, TextAlign::MiddleCenter);
}

void UI::renderNotStarted() const
{
	mRenderer.drawText("Welcome to Arkanoid!", UIConfig::kCenter, Color::Yellow, TextAlign::MiddleCenter);
	mRenderer.drawText("Press SPACE to start", UIConfig::kBottomCenter, Color::White, TextAlign::MiddleCenter);
}

void UI::drawScore(uint32_t score) const
{
	mRenderer.drawText(std::format("Score: {}", score), UIConfig::kScorePos, Color::White);
}

void UI::drawLives(uint32_t lifeCount, GameState state) const
{
	uint32_t renderedLives = lifeCount;
	if (state == GameState::Running || state == GameState::Paused || state == GameState::Won)
		renderedLives--;

	for (size_t i = 0; i < renderedLives; ++i)
	{
		Vector2 position = { UIConfig::kLifeIconBaseX + i * UIConfig::kLifeIconSpacing, UIConfig::kLifeIconY };
		mRenderer.drawFilledCircle(position, GameConfig::kBallRadius, Color::White);
	}
}
