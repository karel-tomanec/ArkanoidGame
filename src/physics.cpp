#include "physics.hpp"
#include <optional>

#include "collision.hpp"

PhysicsHitResult Physics::simulateBallStep(
	const Ball& ball,
	const Vector2& moveVector,
	const Vector2& currentPosition,
	const CollisionContext& collisionContext)
{
	// Test swept-sphere vs. all static objects to find the closest hit

	Sphere sphere{ currentPosition, ball.getRadius() };

	std::optional<HitInfo> closestHit;
	Block* hitBlock = nullptr;
	bool hitPlatform = false;
	bool hitWall = false;

	// Walls
	for (const Wall* wall : collisionContext.walls)
	{
		if (!wall)
			continue;
		if (auto hit = intersectMovingSphereAABB(sphere, moveVector, wall->getAABB()); hit)
		{
			if (!closestHit || lengthSquared(hit->intersection - currentPosition) < lengthSquared(closestHit->intersection - currentPosition))
			{
				closestHit = hit;
				hitBlock = nullptr;
				hitPlatform = false;
				hitWall = true;
			}
		}
	}

	// Blocks
	for (Block* block : collisionContext.blocks)
	{
		if (block->isDestroyed())
			continue;

		if (auto hit = intersectMovingSphereAABB(sphere, moveVector, block->getAABB()); hit)
		{
			if (!closestHit || lengthSquared(hit->intersection - currentPosition) < lengthSquared(closestHit->intersection - currentPosition))
			{
				closestHit = hit;
				hitBlock = block;
				hitPlatform = false;
				hitWall = false;
			}
		}
	}

	// Platform
	if (collisionContext.platform)
	{
		if (auto hit = intersectMovingSphereAABB(sphere, moveVector, collisionContext.platform->getAABB()); hit)
		{
			if (!closestHit || lengthSquared(hit->intersection - currentPosition) < lengthSquared(closestHit->intersection - currentPosition))
			{
				closestHit = hit;
				hitBlock = nullptr;
				hitPlatform = true;
				hitWall = false;
			}
		}
	}

	// If no collision, move full distance and keep direction
	if (!closestHit)
	{
		return PhysicsHitResult{
			.newPosition = currentPosition + moveVector,
			.newDirection = ball.getDirection(),
			.traveled = 1.f,
			.hitBlock = nullptr,
			.hitPlatform = false,
			.hitWall = false
		};
	}

	// Calculate normalized direction to impact point
	Vector2 rawDir = closestHit->intersection - currentPosition;
	float rawLen = length(rawDir);
	Vector2 dir = (rawLen > kEpsilon) ? rawDir / rawLen : ball.getDirection();

	// Reflect movement vector around the hit surface normal
	Vector2 reflected = reflect(dir, closestHit->normal);


	// Add slight influence from moving platform to reflection
	if (collisionContext.platform && hitPlatform)
		reflected = normalize(reflected + collisionContext.platform->getDirection() * 0.001f);

	// Push ball slightly outside the surface to avoid immediate re-collision
	constexpr float pushOut = 0.001f;
	Vector2 newPos = closestHit->intersection + reflected * pushOut;

	return PhysicsHitResult{
		.newPosition = newPos,
		.newDirection = reflected,
		.traveled = closestHit->t,
		.hitBlock = hitBlock,
		.hitPlatform = hitPlatform,
		.hitWall = hitWall
	};
}
