#pragma once
#include <memory>
#include <random>
#include <vector>

#include "ball.hpp"
#include "block.hpp"
#include "particle.hpp"
#include "platform.hpp"
#include "renderer.hpp"
#include "soundPlayer.hpp"
#include "wall.hpp"
#include "SDL3/SDL.h"
#include "collisionContext.hpp"
#include "inputManager.hpp"
#include "gameState.hpp"
#include "gameConfig.hpp"
#include "particleSystem.hpp"
#include "UI.hpp"

class Arkanoid final
{
public:
	Arkanoid();

	~Arkanoid();

	void run();

private:

	void handleEvents();

	void update(double deltaTime);

	void updatePlatform(double deltaTime);

	void updateCollisionContext();

	void updateBallPhysics(double deltaTime, std::vector<Block*>& destroyedBlocks);

	void updateParticles(double deltaTime, const std::vector<Block*>& destroyedBlocks) const;

	void checkGameEndConditions();

	void render() const;

	void renderGameObjects() const;

	void renderUI() const;

	void startGame();

	void restartGame();

	void createWalls();

	void generateLevel();

	void spawnPlatform();

	void spawnBall();

	void setGameState(GameState newState);

	void playSound(SoundPlayer::SoundId soundId) const;

	// Main loop
	bool mIsRunning = false;

	// Window
	SDL_Window* mWindow = nullptr;
	int mWindowWidth = 800;
	int mWindowHeight = 800;
	int kMinWindowSize = 400;

	// Size of the logical game area
	Vector2 mLogicalSize{ 800.0f, 800.0f };

	// Input handling
	InputManager mInputManager;

	// Renderer
	std::unique_ptr<Renderer> mRenderer;
	static constexpr double kTargetFrameTime = 1.0 / 60.0; // 16.67 ms

	// UI
	std::unique_ptr<UI> mUI;

	// Sound player
	std::unique_ptr<SoundPlayer> mSoundPlayer;

	// Game objects
	static constexpr size_t kWallCount = 3;
	std::vector<Wall> mWalls;
	std::unique_ptr<Platform> mPlatform;
	std::unique_ptr<Ball> mBall;
	std::vector<Block> mBlocks;

	// Particle system
	std::unique_ptr<ParticleSystem> mParticleSystem;

	// Collisions
	CollisionContext mCollisionContext;

	// Game state
	uint32_t mScore;
	uint32_t mMaxScore;
	GameState mGameState = GameState::NotStarted;
	uint32_t mLifeCount;
	bool mHitWallPreviously = false;
	bool mHasMoved = false;

	// Random numbers
	std::mt19937 mRng;
};
