#pragma once

#include "NiTMapBase.hpp"
#include "NiTDefaultAllocator.hpp"

template <class T_Key, class T_Data>
class NiTMap : public NiTMapBase<NiTDefaultAllocator<T_Data>, T_Key, T_Data> {
public:
    NiTMap(uint32_t uiHashSize = 37) : NiTMapBase<NiTDefaultAllocator<T_Data>, T_Key, T_Data>(uiHashSize) {};
	~NiTMap() { NiTMapBase<NiTDefaultAllocator<T_Data>, T_Key, T_Data>::RemoveAll(); };

	virtual NiTMapItem<T_Key, T_Data>* NewItem();
	virtual void DeleteItem(NiTMapItem<T_Key, T_Data>* pkItem);
};

template<class T_Key, class T_Data>
inline NiTMapItem<T_Key, T_Data>* NiTMap<T_Key, T_Data>::NewItem() {
	return (NiTMapItem<T_Key, T_Data>*)NiTMapBase<NiTDefaultAllocator<T_Data>, T_Key, T_Data>::m_kAllocator.Allocate();
}

template<class T_Key, class T_Data>
inline void NiTMap<T_Key, T_Data>::DeleteItem(NiTMapItem<T_Key, T_Data>* pkItem) {
	pkItem->m_val = T_Data(0);
	NiTMapBase<NiTDefaultAllocator<T_Data>, T_Key, T_Data>::m_kAllocator.Deallocate(pkItem);
}


static_assert(sizeof(NiTMap<uint32_t, uint32_t>) == 0x10, "Wrong structure size!");