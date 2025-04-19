#pragma once

class AutoScrapHeapBuffer {
public:
	AutoScrapHeapBuffer(uint32_t auiSize, uint32_t auiAlignment = 4);
	~AutoScrapHeapBuffer();

	void* pData;
};