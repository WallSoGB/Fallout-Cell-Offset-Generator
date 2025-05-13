#include "ScrapHeap.hpp"

// GAME - 0x866D10
// GECK - 0x8564E0
ScrapHeapManager* ScrapHeapManager::GetSingleton() {
#ifdef GAME
	return CdeclCall<ScrapHeapManager*>(0x866D10);
#else
	return CdeclCall<ScrapHeapManager*>(0x8564E0);
#endif
}

// GAME - 0xAA5B70
// GECK - 0x856090
void ScrapHeapManager::ReleaseBuffer(void* apAddress, SIZE_T aSize) {
#ifdef GAME
	ThisCall(0xAA5B70, this, apAddress, aSize);
#else
	ThisCall(0x856090, this, apAddress, aSize);
#endif
}

// GAME - 0xAA59B0
// GECK - 0x855ED0
void* ScrapHeapManager::RequestBuffer(SIZE_T& arSize) {
#ifdef GAME
	return ThisCall<void*>(0xAA59B0, this, &arSize);
#else
	return ThisCall<void*>(0x855ED0, this, &arSize);
#endif
}

// GAME - 0xAA53F0
// GECK - 0x855910
ScrapHeap::ScrapHeap() {
#ifdef GAME
	ThisCall(0xAA53F0, this);
#else
	ThisCall(0x855910, this);
#endif
}

// GAME - 0xAA5410
// GECK - 0x855930
ScrapHeap::ScrapHeap(SIZE_T aReserveSize) {
#ifdef GAME
	ThisCall(0xAA5410, this, aReserveSize);
#else
	ThisCall(0x855930, this, aReserveSize);
#endif
}

// GAME - 0xAA5460
// GECK - 0x855980
ScrapHeap::~ScrapHeap() {
#ifdef GAME
	ThisCall(0xAA5460, this);
#else
	ThisCall(0x855980, this);
#endif
}

// GAME - 0xAA54A0
// GECK - 0x8559C0
void* ScrapHeap::Allocate(uint32_t aSize, uint32_t aAlignment) {
#ifdef GAME
	return ThisCall<void*>(0xAA54A0, this, aSize, aAlignment);
#else
	return ThisCall<void*>(0x8559C0, this, aSize, aAlignment);
#endif
}

// GAME - 0xAA5610
// GECK - 0x855B30
void ScrapHeap::Deallocate(void* apMem) {
#ifdef GAME
	ThisCall(0xAA5610, this, apMem);
#else
	ThisCall(0x855B30, this, apMem);
#endif
}

uint32_t ScrapHeap::GetSize() const {
	return reinterpret_cast<uint32_t>(pCommitEnd) - reinterpret_cast<uint32_t>(pBaseAddress);
}

// GAME - 0xAA57B0
// GECK - 0x855CD0
void ScrapHeap::Initialize(SIZE_T aReserveSize) {
#ifdef GAME
	ThisCall(0xAA57B0, this, aReserveSize);
#else
	ThisCall(0x855CD0, this, aReserveSize);
#endif
}
