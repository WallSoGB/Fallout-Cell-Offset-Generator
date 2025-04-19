#include "BSFile.hpp"

// GAME - 0xAFFD80
// GECK - 0x8A0C60
bool BSFile::ChangeBufferSize(uint32_t auiSize) {
#ifdef GAME
	return ThisCall<bool>(0xAFFD80, this, auiSize);
#else
	return ThisCall<bool>(0x8A0C60, this, auiSize);
#endif
}
