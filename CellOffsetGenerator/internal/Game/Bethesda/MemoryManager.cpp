#include "MemoryManager.hpp"
#include "ScrapHeap.hpp"

MemoryManager* MemoryManager::GetSingleton() {
#ifdef GAME
	return (MemoryManager*)0x11F6238;
#else
	return (MemoryManager*)0xF21B5C;
#endif
}

// GAME - 0xAA42E0
// GECK - 0x854540
ScrapHeap* MemoryManager::GetThreadScrapHeap() {
#ifdef GAME
	return ThisCall<ScrapHeap*>(0xAA42E0, this);
#else
	return ThisCall<ScrapHeap*>(0x854540, this);
#endif
}

void* MemoryManager::AllocateScrap(size_t aSize, uint32_t aAlignment) {
	return MemoryManager::GetSingleton()->GetThreadScrapHeap()->Allocate(aSize, aAlignment);
}

void MemoryManager::DeallocateScrap(void* apMemory) {
	MemoryManager::GetSingleton()->GetThreadScrapHeap()->Deallocate(apMemory);
}