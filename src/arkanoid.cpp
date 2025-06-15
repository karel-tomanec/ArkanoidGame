#include "arkanoid.hpp"

#include <ranges>
#include <stdexcept>
#include <format>
#include <random>

#include "color.hpp"
#include "math.hpp"
#include "particle.hpp"
#include "physics.hpp"

Arkanoid::Arkanoid()
	: mRng(std::random_device{}())
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
		throw std::runtime_error(std::format("SDL_Init Error: {}", SDL_GetError()));

	mWindow = SDL_CreateWindow("Arkanoid", mWindowWidth, mWindowHeight, 0);
	if (!mWindow)
		throw std::runtime_error(std::format("SDL_CreateWindow Error: {}", SDL_GetError()));

	mRenderer = std::make_unique<Renderer>(mWindow);

	if (mRenderer)
		mUI = std::make_unique<UI>(*mRenderer);

	mParticleSystem = std::make_unique<ParticleSystem>(mRng);

	mSoundPlayer = std::make_unique<SoundPlayer>();

	createWalls();

	playSound(SoundPlayer::SoundId::Enter);
}

Arkanoid::~Arkanoid()
{
	if (mRenderer)
		mRenderer.reset();

	if (mSoundPlayer)
		mSoundPlayer.reset();

	if (mWindow)
	{
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
	SDL_Quit();
}

void Arkanoid::run()
{
	mIsRunning = true;

	const auto perfFrequency = SDL_GetPerformanceFrequency();
	auto lastFrameTime = SDL_GetPerformanceCounter();

	while (mIsRunning)
	{
		// Measure elapsed time
		auto frameStartTime = SDL_GetPerformanceCounter();
		double deltaTime = static_cast<double>(frameStartTime - lastFrameTime) / perfFrequency;
		lastFrameTime = frameStartTime;

		handleEvents();
		update(deltaTime);
		render();

		// Frame limiting
		auto frameEndTime = SDL_GetPerformanceCounter();
		double frameElapsed = static_cast<double>(frameEndTime - frameStartTime) / perfFrequency;
		double frameDelay = kTargetFrameTime - frameElapsed;
		if (frameDelay > 0.0)
			SDL_Delay(static_cast<Uint32>(frameDelay * 1000.0));
	}
}

void Arkanoid::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
			mIsRunning = false;
		mInputManager.handleEvent(event);
	}

	if (mInputManager.isKeyPressed(SDLK_ESCAPE))
		mIsRunning = false;

	if (mInputManager.isKeyPressed(SDLK_R))
	{
		restartGame();
		playSound(SoundPlayer::SoundId::Click);
	}

	if (mInputManager.isKeyPressed(SDLK_SPACE))
	{
		switch (mGameState)
		{
			case GameState::NotStarted:
			case GameState::GameOver:
			case GameState::Won:
				startGame();
				break;
			case GameState::Running:
				setGameState(GameState::Paused);
				break;
			case GameState::Paused:
				setGameState(GameState::Running);
				break;
			case GameState::AwaitingServe:
				spawnBall();
				setGameState(GameState::Running);
				break;
		}
		playSound(SoundPlayer::SoundId::Click);
	}

	MoveDirection moveDir = MoveDirection::None;
	if (mInputManager.isKeyHeld(SDLK_LEFT))
		moveDir = MoveDirection::Left;
	if (mInputManager.isKeyHeld(SDLK_RIGHT))
		moveDir = MoveDirection::Right;

	mHasMoved |= moveDir != MoveDirection::None;
	if (mPlatform)
		mPlatform->handleInput(moveDir);

	mInputManager.clear();

}

void Arkanoid::update(double deltaTime)
{
	if (mGameState == GameState::Paused || mGameState == GameState::NotStarted)
		return;

	std::vector<Block*> destroyedBlocks;
	updatePlatform(deltaTime);

	if (mGameState != GameState::GameOver)
	{
		updateCollisionContext();
		updateBallPhysics(deltaTime, destroyedBlocks);
	}
	updateParticles(deltaTime, destroyedBlocks);
	checkGameEndConditions();
}

void Arkanoid::updatePlatform(double deltaTime)
{
	if (!mPlatform) return;

	auto nextPosition = mPlatform->getPosition() + mPlatform->getDirection() * static_cast<float>(deltaTime);
	const auto& size = mPlatform->getSize();

	const auto leftWall = mWalls[0].getAABB().max.x;
	const auto rightWall = mWalls[2].getAABB().min.x;

	bool hitWall = false;
	if (nextPosition.x - size.x * 0.5f < leftWall)
	{
		nextPosition.x = leftWall + size.x * 0.5f;
		hitWall = true;
	}

	if (nextPosition.x + size.x * 0.5f > rightWall)
	{
		nextPosition.x = rightWall - size.x * 0.5f;
		hitWall = true;
	}

	mPlatform->setPosition(nextPosition);

	if (!mHitWallPreviously && hitWall)
		playSound(SoundPlayer::SoundId::HitWall);

	mHitWallPreviously = hitWall;
}

void Arkanoid::updateCollisionContext()
{
	// Add blocks in reverse order to ensure the blocks in the front are checked first
	mCollisionContext.blocks.clear();
	for (auto& mBlock : std::ranges::reverse_view(mBlocks))
	{
		if (!mBlock.isDestroyed())
			mCollisionContext.blocks.push_back(&mBlock);
	}

	mCollisionContext.platform = mPlatform.get();
}

void Arkanoid::updateBallPhysics(double deltaTime, std::vector<Block*>& destroyedBlocks)
{
	if (!mBall)
		return;

	float speed = mBall->getSpeed();
	float remainingDistance = speed * static_cast<float>(deltaTime);

	// Continuous collision detection loop
	// Simulate step-by-step movement until the full distance is consumed.
	while (remainingDistance > 0.f)
	{
		Vector2 moveVec = mBall->getDirection() * remainingDistance;

		// Simulate a single movement step and compute potential collision response
		PhysicsHitResult hit = Physics::simulateBallStep(
			*mBall,
			moveVec,
			mBall->getPosition(),
			mCollisionContext
		);

		mBall->setPosition(hit.newPosition);
		mBall->setDirection(hit.newDirection);

		// Reduce remaining distance by how far the ball moved before the collision (fractional)
		remainingDistance -= hit.traveled * remainingDistance;
		remainingDistance = std::max(remainingDistance, 0.f); // avoid negative values

		if (hit.hitPlatform && mCollisionContext.platform)
			mBall->resetSpeedAndColor();

		// Handle block destruction logic
		if (hit.hitBlock)
		{
			hit.hitBlock->tryDestroy();
			if (hit.hitBlock->isDestroyed())
			{
				mScore += hit.hitBlock->getScore();
				destroyedBlocks.push_back(hit.hitBlock);
			}
			if (hit.hitBlock->getType() == BlockType::Booster)
				mBall->setSpeed(mBall->getSpeed() + GameConfig::kBallSpeedIncrement);

			mBall->setColor(hit.hitBlock->getColor());
		}

		if (hit.hitBlock || hit.hitPlatform || hit.hitWall)
			playSound(SoundPlayer::SoundId::Bounce);
		else
			break;

	}
}

void Arkanoid::checkGameEndConditions()
{
	if (!mBall)
		return;

	if (mScore == mMaxScore)
	{
		setGameState(GameState::Won);
		mBall.reset();
	}
	else if (mGameState == GameState::Running && mBall->getPosition().y > static_cast<float>(mWindowHeight))
	{
		mLifeCount--;
		mBall.reset();
		if (mLifeCount == 0)
			setGameState(GameState::GameOver);
		else
			setGameState(GameState::AwaitingServe);
	}
}

void Arkanoid::render() const
{
	if (!mRenderer)
		return;

	mRenderer->clearScreen();

	renderGameObjects();
	renderUI();

	mRenderer->presentFrame();
}

void Arkanoid::renderGameObjects() const
{
	for (auto& wall : mWalls)
		wall.render(*mRenderer);

	if (mBall)
		mBall->render(*mRenderer);

	if (mPlatform)
		mPlatform->render(*mRenderer);

	for (const auto& block : mBlocks)
		block.render(*mRenderer);

	if (mParticleSystem)
		mParticleSystem->render(*mRenderer);
}

void Arkanoid::renderUI() const
{
	if (mUI)
		mUI->render(mGameState, mScore, mLifeCount, mHasMoved);
}

void Arkanoid::startGame()
{
	// Reset game state
	mGameState = GameState::AwaitingServe;
	mScore = 0;
	mMaxScore = 0;
	mLifeCount = 3;
	mHasMoved = false;

	generateLevel();
	spawnPlatform();
}

void Arkanoid::restartGame()
{
	mBall.reset();
	startGame();
}

void Arkanoid::createWalls()
{
	mWalls.reserve(kWallCount);
	mWalls.emplace_back(Vector2{ 5.f, static_cast<float>(mWindowHeight) * 0.5f }, Vector2{ 10.f, static_cast<float>(mWindowHeight) }, Color::Gray); // Left wall
	mWalls.emplace_back(Vector2{ static_cast<float>(mWindowWidth) * 0.5f, 5.f }, Vector2{ static_cast<float>(mWindowWidth), 10.f }, Color::Gray); // Top wall
	mWalls.emplace_back(Vector2{ static_cast<float>(mWindowWidth) - 5.f, static_cast<float>(mWindowHeight) * 0.5f }, Vector2{ 10.f, static_cast<float>(mWindowHeight) }, Color::Gray); // Right wall

	for (uint32_t i = 0; i < mWalls.size(); i++)
		mCollisionContext.walls[i] = &mWalls[i];
}

void Arkanoid::generateLevel()
{
	mBlocks.clear();
	mBlocks.reserve(GameConfig::kBlockColumnCount * GameConfig::kBlockRowCount);

	constexpr std::array blockTypes =
	{
		BlockType::Normal,
		BlockType::Booster,
		BlockType::Reinforced
	};

	constexpr std::array blockProbabilities =
	{
		0.7f,
		0.2f,
		0.1f,
	};

	// Use cumulative probabilities to choose block types
	std::vector<float> cumulativeProbabilities;
	cumulativeProbabilities.reserve(blockProbabilities.size());
	float cumulativeSum = 0.0f;
	for (const auto& prob : blockProbabilities)
	{
		cumulativeSum += prob;
		cumulativeProbabilities.push_back(cumulativeSum);
	}

	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	for (size_t i = 0; i < GameConfig::kBlockColumnCount; ++i)
	{
		for (size_t j = 0; j < GameConfig::kBlockRowCount; ++j)
		{
			Vector2 position = { static_cast<float>(10 + i * GameConfig::kBlockSize.x + GameConfig::kBlockSize.x * 0.5f), static_cast<float>(10 + j * GameConfig::kBlockSize.y + GameConfig::kBlockSize.y * 0.5f) };

			float randomValue = dist(mRng);
			BlockType blockType = BlockType::Normal;
			for (size_t k = 0; k < cumulativeProbabilities.size(); ++k)
			{
				if (randomValue <= cumulativeProbabilities[k])
				{
					blockType = blockTypes[k];
					break;
				}
			}

			mBlocks.emplace_back(position, GameConfig::kBlockSize, blockType);
			mMaxScore += mBlocks.back().getScore(); // Update max score based on block type
		}
	}
}

void Arkanoid::spawnPlatform()
{
	if (!mPlatform)
		mPlatform = std::make_unique<Platform>(GameConfig::kDefaultPlatformStartPosition, GameConfig::kPlatformSize, Color::White);
}

void Arkanoid::spawnBall()
{
	Vector2 ballStartPosition = { mPlatform->getPosition().x, mPlatform->getPosition().y - GameConfig::kBallRadius - mPlatform->getSize().y * 0.5f - 1.f };
	mBall = std::make_unique<Ball>(ballStartPosition, GameConfig::kBallRadius, GameConfig::kDefaultBallSpeed);

	// Set random angle for the ball's initial direction
	constexpr float spreadAngle = 30.f;
	static std::uniform_real_distribution angleDist(-spreadAngle, spreadAngle);

	float angleDeg = angleDist(mRng);
	float angleRad = angleDeg * (pi / 180.0f);

	Vector2 dir = { std::sin(angleRad), -std::cos(angleRad) };
	mBall->setDirection(dir);
	playSound(SoundPlayer::SoundId::Start);
}

void Arkanoid::setGameState(GameState newState)
{
	mGameState = newState;
	switch (newState)
	{
		case GameState::Won: playSound(SoundPlayer::SoundId::Win); break;
		case GameState::GameOver: playSound(SoundPlayer::SoundId::GameOver); break;
		case GameState::AwaitingServe: playSound(SoundPlayer::SoundId::Fall); break;
		default: break;
	}
}

void Arkanoid::playSound(SoundPlayer::SoundId soundId) const
{
	if (mSoundPlayer)
		mSoundPlayer->play(soundId);
}

void Arkanoid::updateParticles(double deltaTime, const std::vector<Block*>& destroyedBlocks) const
{
	if (!mParticleSystem)
		return;

	mParticleSystem->update(deltaTime);

	for (const auto& b : destroyedBlocks)
		mParticleSystem->emitFromBlock(*b);

	if (mBall)
		mParticleSystem->emitFromBall(*mBall);

	if (mPlatform)
		mParticleSystem->emitFromPlatform(*mPlatform);
}
