#pragma once
#include <cstdint>

enum class GameState : uint8_t
{
	NotStarted,
	Running,
	Paused,
	AwaitingServe,
	GameOver,
	Won
};
