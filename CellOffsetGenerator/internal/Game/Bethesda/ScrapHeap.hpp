#pragma once

#include <BSSpinLock.hpp>

class ScrapHeapManager {
public:
	struct Buffer {
		LPVOID		pData;
		uint32_t	uiSize;
	};

	Buffer				kBuffers[64];
	uint32_t			uiBufferCount;
	uint32_t			uiBufferAllocBytes;
	uint32_t			uiSysAllocBytes;
	DWORD				dword208[5];
	BSSpinLock			kLock;

	static ScrapHeapManager* GetSingleton();
	void	ReleaseBuffer(void* apAddress, SIZE_T aSize);
	void*	RequestBuffer(SIZE_T& arSize);
};

ASSERT_SIZE(ScrapHeapManager, 0x240);

class ScrapHeap {
public:
	ScrapHeap();
	ScrapHeap(SIZE_T aReserveSize);
	~ScrapHeap();

	struct Block {
		enum Flags {
			DEALLOCATED = 0x80000000,
		};

		uint32_t	uiSizeAndFlags;
		Block*		pPrevious;

		bool	IsFree() const { return (uiSizeAndFlags & DEALLOCATED) != 0; }
		void	SetFree(bool abFree) { if (abFree) uiSizeAndFlags |= DEALLOCATED; else uiSizeAndFlags &= ~DEALLOCATED; }
		uint32_t  GetSize() const { return uiSizeAndFlags & ~DEALLOCATED; }
	};

	struct FreeBlock : Block {
		FreeBlock* pLeft;
		FreeBlock* pRight;
	};


	char*	pBaseAddress;
	char*	pCommitStart;
	char*	pCommitEnd;
	Block*	pLastBlock;

	template <typename T>
	T* AllocateT(uint32_t aCount = 1) {
		return (T*)Allocate(aCount * sizeof(T), alignof(T));
	}
	template <typename T>
	T* AllocateT(uint32_t aCount, uint32_t aAlignment) {
		return (T*)Allocate(aCount * sizeof(T), aAlignment);
	}
	void*	Allocate(uint32_t aSize, uint32_t aAlignment = 4);
	void	Deallocate(void* apMem);

	uint32_t	GetSize() const;

private:
	friend class ScrapHeapReplacer;
	void	Initialize(SIZE_T aReserveSize);
};

ASSERT_SIZE(ScrapHeap, 0x10);
ASSERT_SIZE(ScrapHeap::Block, 0x8);