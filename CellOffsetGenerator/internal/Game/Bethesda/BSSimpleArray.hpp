#pragma once

#include "BSMemObject.hpp"

template <typename T, uint32_t RESIZE_SIZE = 1024>
class BSSimpleArray {
public:
	BSSimpleArray();
	virtual			~BSSimpleArray();
	virtual T*		Allocate(uint32_t auiCount);
	virtual void    Deallocate(T* apData);
	virtual T*		Reallocate(T* apData, uint32_t auiCount);

	T*			pBuffer;
	uint32_t	uiSize;
	uint32_t	uiAllocSize;
};

ASSERT_SIZE(BSSimpleArray<uint32_t>, 0x10);