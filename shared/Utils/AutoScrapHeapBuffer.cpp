#include "AutoScrapHeapBuffer.hpp"
#include "MemoryManager.hpp"
#include "ScrapHeap.hpp"

AutoScrapHeapBuffer::AutoScrapHeapBuffer(uint32_t auiSize, uint32_t auiAlignment, ScrapHeap* apHeap) : pData(nullptr) {
	if (apHeap)
		pHeap = apHeap;
	else
		pHeap = MemoryManager::GetSingleton()->GetThreadScrapHeap();

	if (auiSize)
		pData = pHeap->Allocate(auiSize, auiAlignment);
}

AutoScrapHeapBuffer::~AutoScrapHeapBuffer() {
	if (pData)
		pHeap->Deallocate(pData);
}