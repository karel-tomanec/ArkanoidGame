#pragma once

namespace GameConfig
{
	// Block
	constexpr Vector2 kBlockSize{ 65.f, 30.f };
	constexpr size_t kBlockColumnCount = 12;
	constexpr size_t kBlockRowCount = 5;
	constexpr uint32_t kParticlesPerBlock = 40;
	// Ball
	constexpr float kDefaultBallSpeed = 500.f;
	constexpr float kBallSpeedIncrement = 100.f;
	constexpr float kBallRadius = 10.f;
	// Platform
	constexpr Vector2 kPlatformSize = { 60, 10 };
	constexpr Vector2 kDefaultPlatformStartPosition = { 400, 700 };
}