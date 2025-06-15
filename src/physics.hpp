#pragma once

#include "ball.hpp"
#include "collisionContext.hpp"

struct PhysicsHitResult
{
	Vector2 newPosition;
	Vector2 newDirection;
	float traveled;
	Block* hitBlock = nullptr;
	bool hitPlatform = false;
	bool hitWall = false;
};

namespace Physics
{
	PhysicsHitResult simulateBallStep(
		const Ball& ball,
		const Vector2& moveVector,
		const Vector2& currentPosition,
		const CollisionContext& collisionContext);
}