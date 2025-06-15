#pragma once

#include <vector>
#include <array>

#include "wall.hpp"
#include "platform.hpp"
#include "block.hpp"

struct CollisionContext
{
	std::array<const Wall*, 3> walls; // left, top, right
	const Platform* platform;
	std::vector<Block*> blocks;
};