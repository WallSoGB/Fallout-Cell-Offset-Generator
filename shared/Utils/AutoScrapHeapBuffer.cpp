#include "AutoScrapHeapBuffer.hpp"
#include "MemoryManager.hpp"
#include "ScrapHeap.hpp"

AutoScrapHeapBuffer::AutoScrapHeapBuffer(uint32_t auiSize, uint32_t auiAlignment) : pData(nullptr) {
	if (auiSize)
		pData = MemoryManager::GetSingleton()->GetThreadScrapHeap()->Allocate(auiSize, auiAlignment);
}

AutoScrapHeapBuffer::~AutoScrapHeapBuffer() {
	if (pData)
		MemoryManager::GetSingleton()->GetThreadScrapHeap()->Deallocate(pData);
}