#pragma once

#include "Tile.hpp"

class TileRect : public Tile {
public:
	uint32_t	unk38;
};

ASSERT_SIZE(TileRect, 0x3C);