#pragma once

#include "NiMemObject.hpp"

typedef void* NiTMapIterator;

template <class T_Key, class T_Data> 
class NiTMapItem : public NiMemObject {
public:
    NiTMapItem* m_pkNext;
    T_Key       m_key;
    T_Data      m_val;
};

template <class Allocator, class T_Key, class T_Data>
class NiTMapBase : public NiMemObject {
public:
    typedef NiTMapItem<T_Key, T_Data> Entry;

    struct AntiBloatAllocator : public Allocator {
        uint32_t m_uiCount;
    };

    NiTMapBase(uint32_t uiHashSize = 37) {
        m_uiHashSize = uiHashSize;
        m_kAllocator.m_uiCount = 0;

        m_ppkHashTable = NiAlloc<NiTMapItem<T_Key, T_Data>*>(uiHashSize);
        memset(m_ppkHashTable, 0, uiHashSize * sizeof(NiTMapItem<T_Key, T_Data>*));
    };
	NiTMapBase(const NiTMapBase&) = delete;
	NiTMapBase& operator=(const NiTMapBase&) = delete;

    virtual                             ~NiTMapBase();
    virtual uint32_t                    KeyToHashIndex(const T_Key key) const;
    virtual bool                        IsKeysEqual(const T_Key key1, const T_Key key2) const;
    virtual void                        SetValue(NiTMapItem<T_Key, T_Data>* apItem, T_Key key, T_Data data);
    virtual void                        ClearValue(NiTMapItem<T_Key, T_Data>* apItem);
    virtual NiTMapItem<T_Key, T_Data>*  NewItem() = 0;
    virtual void                        DeleteItem(NiTMapItem<T_Key, T_Data>* apItem) = 0;

    uint32_t                    m_uiHashSize;
    NiTMapItem<T_Key, T_Data>** m_ppkHashTable;
    AntiBloatAllocator          m_kAllocator;

    uint32_t GetCount() const { return m_kAllocator.m_uiCount; }
	bool IsEmpty() const { return m_kAllocator.m_uiCount == 0; }

    bool GetAt(const T_Key key, T_Data& dataOut) const {
        uint32_t uiHashIndex = KeyToHashIndex(key);
        NiTMapItem<T_Key, T_Data>* pItem = m_ppkHashTable[uiHashIndex];
        while (pItem) {
            if (IsKeysEqual(pItem->m_key, key)) {
                dataOut = pItem->m_val;
                return true;
            }
            pItem = pItem->m_pkNext;
        }
        return false;
    }

    void SetAt(T_Key key, T_Data data) {
        uint32_t uiHashIndex = KeyToHashIndex(key);
        NiTMapItem<T_Key, T_Data>* pItem = m_ppkHashTable[uiHashIndex];
        while (pItem) {
            if (IsKeysEqual(pItem->m_key, key)) {
                pItem->m_val = data;
                return;
            }
            pItem = pItem->m_pkNext;
        }
        NiTMapItem<T_Key, T_Data>* pNewEntry = NewItem();
        SetValue(pNewEntry, key, data);
        pNewEntry->m_pkNext = m_ppkHashTable[uiHashIndex];
        m_ppkHashTable[uiHashIndex] = pNewEntry;
        m_kAllocator.m_uiCount++;
    }

    bool Insert(const NiTMapItem<T_Key, T_Data>* apNewEntry) {
        // game code does not appear to care about ordering of entries in buckets
        uint32_t bucket = apNewEntry->m_key % m_uiHashSize;
        NiTMapItem<T_Key, T_Data>* prev = nullptr;
        for (NiTMapItem<T_Key, T_Data>* cur = m_ppkHashTable[bucket]; cur; cur = cur->m_pkNext) {
            if (cur->m_key == apNewEntry->m_key) {
                return false;
            }
            else if (!cur->m_pkNext) {
                prev = cur;
                break;
            }
        }

        if (prev) {
            prev->m_pkNext = apNewEntry;
        }
        else {
            m_ppkHashTable[bucket] = apNewEntry;
        }

        m_uiHashSize++;
        return true;
    }

    // 0x405430
    bool RemoveAt(const T_Key key) {
        uint32_t uiIndex = KeyToHashIndex(key);
        NiTMapItem<T_Key, T_Data>* pItem = m_ppkHashTable[uiIndex];

        if (!pItem)
            return false;

        if (IsKeysEqual(key, pItem->m_key)) {
            m_ppkHashTable[uiIndex] = pItem->m_pkNext;
            ClearValue(pItem);
            DeleteItem(pItem);
            --m_kAllocator.m_uiCount;
            return true;
        }

        NiTMapItem<T_Key, T_Data>* pPrev = pItem;
        NiTMapItem<T_Key, T_Data>* i = pItem->m_pkNext;

        for (i; i && !IsKeysEqual(key, i->m_key); i = i->m_pkNext)
            pPrev = i;

        if (!i)
            return false;

        pPrev->m_pkNext = i->m_pkNext;
        ClearValue(i);
        DeleteItem(i);
        --m_kAllocator.m_uiCount;
        return true;
    }

    void RemoveAll() {
        for (uint32_t i = 0; i < m_uiHashSize; i++) {
            while (m_ppkHashTable[i]) {
                NiTMapItem<T_Key, T_Data>* pItem = m_ppkHashTable[i];
                m_ppkHashTable[i] = m_ppkHashTable[i]->m_pkNext;
                ClearValue(pItem);
                DeleteItem(pItem);
            }
        }

        m_kAllocator.m_uiCount = 0;
    }

    NiTMapIterator GetFirstPos() const {
        for (uint32_t i = 0; i < m_uiHashSize; i++) {
            if (m_ppkHashTable[i])
                return m_ppkHashTable[i];
        }
        return nullptr;
    }

    void GetNext(NiTMapIterator& pos, T_Key& key, T_Data& val) {
        NiTMapItem<T_Key, T_Data>* pItem = (NiTMapItem<T_Key, T_Data>*) pos;

        key = pItem->m_key;
        val = pItem->m_val;

        if (pItem->m_pkNext) {
            pos = pItem->m_pkNext;
            return;
        }

        uint32_t i = KeyToHashIndex(pItem->m_key);
        for (++i; i < m_uiHashSize; i++) {
            pItem = m_ppkHashTable[i];
            if (pItem) {
                pos = pItem;
                return;
            }
        }

        pos = nullptr;
    }
};

template<class Allocator, class T_Key, class T_Data>
inline NiTMapBase<Allocator, T_Key, T_Data>::~NiTMapBase() {
    RemoveAll();
    NiFree(m_ppkHashTable);
}

template<class Allocator, class T_Key, class T_Data>
inline uint32_t NiTMapBase<Allocator, T_Key, T_Data>::KeyToHashIndex(const T_Key key) const {
    return (uint32_t)(((size_t)key) % m_uiHashSize);
}

template<class Allocator, class T_Key, class T_Data>
inline bool NiTMapBase<Allocator, T_Key, T_Data>::IsKeysEqual(const T_Key key1, const T_Key key2) const {
    return key1 == key2;
}

template<class Allocator, class T_Key, class T_Data>
inline void NiTMapBase<Allocator, T_Key, T_Data>::SetValue(NiTMapItem<T_Key, T_Data>* pItem, T_Key key, T_Data data) {
    pItem->m_key = key;
    pItem->m_val = data;
}

template<class Allocator, class T_Key, class T_Data>
inline void NiTMapBase<Allocator, T_Key, T_Data>::ClearValue(NiTMapItem<T_Key, T_Data>* pItem) {
}
