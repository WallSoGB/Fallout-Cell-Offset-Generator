#include "Tile.hpp"

// GAME - 0xA01350
void Tile::SetValue(uint32_t auiID, const char* apString, bool abPropagate) {
	ThisCall(0xA01350, this, auiID, apString, abPropagate);
}

// GAME - 0xA012D0
void Tile::SetValue(uint32_t auiID, float afValue, bool abPropagate) {
	ThisCall(0xA012D0, this, auiID, afValue, abPropagate);
}

// GAME - 0xA01B00
Tile* Tile::ReadFile(const char* apFileName) {
	return ThisCall<Tile*>(0xA01B00, this, apFileName);
}

// GAME - 0xA087D0
void Tile::SetParent(Tile* apParent, Tile* apPrevious) {
	ThisCall(0xA087D0, this, apParent, apPrevious);
}

// GAME - 0x9FF690
void Tile::Release() {
	ThisCall(0x9FF690, this);
}

// GAME - 0xA08B20
Tile* Tile::GetChildByName(Tile* apTile, const char* apName) {
	return CdeclCall<Tile*>(0xA08B20, apTile, apName);
}

uint32_t Tile::TextToTrait(const char* apTraitName) {
	return CdeclCall<uint32_t>(0xA01860, apTraitName);
}