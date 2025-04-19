#pragma once

class ScrapHeap;

struct MemoryManager {
	static MemoryManager* GetSingleton();
    
	ScrapHeap* GetThreadScrapHeap();

    static void* AllocateScrap(size_t aSize, uint32_t aAlignment);
    static void DeallocateScrap(void* apMemory);
};