#pragma once

class ScrapHeap;

class AutoScrapHeapBuffer {
public:
	AutoScrapHeapBuffer(uint32_t auiSize, uint32_t auiAlignment = 4, ScrapHeap* apHeap = nullptr);
	~AutoScrapHeapBuffer();

	ScrapHeap*	pHeap = nullptr;
	void*		pData = nullptr;
};