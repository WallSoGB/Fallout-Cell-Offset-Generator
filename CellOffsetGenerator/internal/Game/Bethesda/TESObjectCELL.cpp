#include "TESObjectCELL.hpp"

// GAME - 0x587410
// GECK - 0x665510
uint32_t TESObjectCELL::GetCoord(int16_t x, int16_t y) {
    return (int32_t(x) << 16) | uint16_t(y);
}

// GAME - 0x544280
// GECK - 0x625890
uint32_t TESObjectCELL::CalcExtGroupBlockKey(int32_t aiX, int32_t aiY) {
    return GetCoord(aiX >> 5, aiY >> 5);
}

// GAME - 0x5442C0
// GECK - 0x6258B0
uint32_t TESObjectCELL::CalcExtGroupSubBlockKey(int32_t aiX, int32_t aiY) {
    return GetCoord(aiX >> 3, aiY >> 3);
}