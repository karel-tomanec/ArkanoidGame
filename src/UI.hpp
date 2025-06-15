#pragma once

#include "renderer.hpp"
#include "gameState.hpp"

class UI
{
public:
	UI(Renderer& renderer);

	void render(GameState gameState, uint32_t score, uint32_t lifeCount, bool hasMoved) const;

private:
	void renderRunning(uint32_t score, uint32_t lifeCount, bool hasMoved) const;
	void renderPaused(uint32_t score, uint32_t lifeCount) const;
	void renderAwaitingServe(uint32_t score, uint32_t lifeCount) const;
	void renderGameOver(uint32_t score) const;
	void renderWon(uint32_t score, uint32_t lifeCount) const;
	void renderNotStarted() const;

	void drawLives(uint32_t lifeCount, GameState gameState) const;
	void drawScore(uint32_t score) const;

	Renderer& mRenderer;
};