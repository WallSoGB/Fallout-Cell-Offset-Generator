#pragma once

#include "BSMemObject.hpp"


template <class T_Data>
class BSSimpleList {
public:
	BSSimpleList() : m_item(0), m_pkNext(0) { }
	BSSimpleList(const T_Data& item) : m_item(item), m_pkNext(0) {}
	BSSimpleList(const T_Data& item, BSSimpleList<T_Data>* next) : m_item(item), m_pkNext(next) {}
	BSSimpleList(BSSimpleList<T_Data>& entry) : m_item(entry.m_item), m_pkNext(0) {}
	~BSSimpleList() { RemoveAll(); }

	T_Data					m_item;
	BSSimpleList<T_Data>*	m_pkNext;

	__forceinline const T_Data GetItem() const { return m_item; }
	__forceinline T_Data GetItem() { return m_item; }
	__forceinline void SetItem(const T_Data aItem) { m_item = aItem; }

	__forceinline const BSSimpleList<T_Data>* GetNext() const { return m_pkNext; }
	__forceinline BSSimpleList<T_Data>* GetNext() { return m_pkNext; }
	__forceinline void SetNext(BSSimpleList<T_Data>* aNext) { m_pkNext = aNext; }

	// 0x8256D0
	bool IsEmpty() const { return !m_pkNext && !m_item; }

	// Custom
	const BSSimpleList<T_Data>* GetHead() const { return this; }
	BSSimpleList<T_Data>* GetHead() { return this; }

	// Custom
	BSSimpleList<T_Data>* GetTail() const {
		BSSimpleList<T_Data>* kIter;
		for (kIter = const_cast<BSSimpleList<T_Data>*>(this); kIter->GetNext(); kIter = kIter->GetNext()) {}
		return kIter;
	}

	class Iterator {
	public:
		Iterator(BSSimpleList<T_Data>* node) : m_node(node) {}

		T_Data& operator*() { return m_node->m_item; }
		const T_Data& operator*() const { return m_node->m_item; }

		Iterator& operator++() {
			if (m_node)
				m_node = m_node->m_pkNext;
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return m_node != other.m_node;
		}

	private:
		BSSimpleList<T_Data>* m_node;
	};

	Iterator begin() { return Iterator(this); }
	Iterator end() { return Iterator(nullptr); }

	// 0x631540
	void AddHead(const T_Data& aItem) {
		if (!aItem)
			return;

		BSSimpleList<T_Data>* pHead = GetHead();

		if (pHead->GetItem()) {
			BSSimpleList<T_Data>* pNewEntry = new BSSimpleList<T_Data>(*pHead);
			pNewEntry->SetNext(pHead->GetNext());
			pHead->SetNext(pNewEntry);
			pHead->SetItem(aItem);

		}
		else {
			pHead->SetItem(aItem);
		}
	}

	// 0xAF25B0, 0x905820, 0xB63BF0
	void AddTail(const T_Data& aItem) {
		if (!aItem)
			return;

		BSSimpleList<T_Data>* pTail = GetTail();

		if (pTail->GetItem()) {
			pTail->SetNext(new BSSimpleList<T_Data>(aItem));
		}
		else {
			pTail->SetItem(aItem);
		}
	}

	// 0x5F65D0
	__forceinline bool IsInList(T_Data& aItem) const {
		const BSSimpleList<T_Data>* pIter = this;
		while (pIter && pIter->GetItem() != aItem)
			pIter = pIter->GetNext();
		return pIter != 0;
	}

	__forceinline BSSimpleList<T_Data>* GetPos(T_Data& aItem) const {
		BSSimpleList<T_Data>* pIter = const_cast<BSSimpleList<T_Data>*>(this);
		while (pIter && pIter->GetItem() != aItem)
			pIter = pIter->GetNext();
		return pIter;
	}

	// 0x5AE380
	__forceinline uint32_t ItemsInList() const {
		uint32_t uiCount = 0;
		const BSSimpleList<T_Data>* pIter = this;
		while (pIter) {
			if (pIter->GetItem())
				++uiCount;

			pIter = pIter->GetNext();
		}
		return uiCount;
	}

	// 0xB64EC0
	void RemoveAll() {
		if (GetNext()) {
			BSSimpleList<T_Data>* pNext = 0;
			do {
				auto pCurrNode = GetNext();
				pNext = pCurrNode->GetNext();
				pCurrNode->SetNext(0);
				delete m_pkNext;
				SetNext(pNext);
			} while (pNext);
		}
		SetItem(0);
	}

	// 0xB99730
	void RemoveHead() {
		auto pNext = GetNext();
		if (pNext) {
			SetNext(pNext->GetNext());
			SetItem(pNext->GetItem());
			pNext->SetNext(nullptr);
			delete pNext;
		}
		else {
			SetItem(0);
		}
	}

	void RemoveTail() {
		BSSimpleList<T_Data>* pTail = GetTail();
		if (pTail) {
			BSSimpleList<T_Data>* pIter = this;
			while (pIter->GetNext() != pTail)
				pIter = pIter->GetNext();

			pIter->SetNext(0);
			delete pTail;
		}
		else {
			SetItem(0);
		}
	}

	// 0x905330
	void Remove(const T_Data& aItem) {
		if (!aItem || IsEmpty())
			return;

		BSSimpleList<T_Data>* pIter = this;
		BSSimpleList<T_Data>* pPrev = this;

		while (pIter && pIter->GetItem() != aItem) {
			pPrev = pIter;
			pIter = pIter->GetNext();
		}

		if (!pIter)
			return;

		if (pIter == this) {
			if (GetNext()) {
				auto pNext = GetNext();
				SetNext(pNext->GetNext());
				SetItem(pNext->GetItem());
				pNext->SetNext(nullptr);
				delete pNext;
			}
			else {
				SetItem(0);
			}
		}
		else {
			pPrev->SetNext(pIter->GetNext());
			pIter->SetNext(0);
			delete pIter;
		}
	}

	// Custom methods

	BSSimpleList<T_Data>* GetAt(uint32_t auiIndex) const {
		uint32_t i = 0;
		BSSimpleList<T_Data>* pIter = const_cast<BSSimpleList<T_Data>*>(this);
		while (pIter) {
			if (i == auiIndex)
				return pIter;

			pIter = pIter->GetNext();
			++i;
		}
		return nullptr;
	}

	int32_t GetIndexOf(const T_Data& aItem) const {
		int32_t iIndex = 0;
		const BSSimpleList<T_Data>* pIter = this;
		while (pIter) {
			if (pIter->GetItem() == aItem)
				return iIndex;

			pIter = pIter->GetNext();
			++iIndex;
		}
		return -1;
	}

	template <typename FUNC>
	int32_t GetIndexOf(const FUNC&& func) const {
		int32_t iIndex = 0;
		const BSSimpleList<T_Data>* pIter = this;
		while (pIter) {
			if (func(pIter))
				return iIndex;

			pIter = pIter->GetNext();
			++iIndex;
		}
		return -1;
	}

	BSSimpleList<T_Data>* Find(const T_Data& aItem) const {
		BSSimpleList<T_Data>* pIter = const_cast<BSSimpleList<T_Data>*>(this);
		while (pIter) {
			if (pIter->GetItem() == aItem)
				return pIter;

			pIter = pIter->GetNext();
		}
		return nullptr;
	}

	template <typename FUNC>
	BSSimpleList<T_Data>* Find(const FUNC&& func) const {
		BSSimpleList<T_Data>* pIter = const_cast<BSSimpleList<T_Data>*>(this);
		while (pIter) {
			if (func(pIter))
				return pIter;

			pIter = pIter->GetNext();
		}
		return nullptr;
	}

	// 0x5F65D0
	bool IsInList(const T_Data& aItem) const {
		const BSSimpleList<T_Data>* pIter = this;
		while (pIter && pIter->GetItem() != aItem)
			pIter = pIter->GetNext();

		return pIter != nullptr;
	}

	template <typename FUNC>
	bool IsInList(FUNC&& func) const {
		return Find(func) != nullptr;
	}

	template <typename FUNC, typename... ARGS>
	void ForEach(FUNC&& func, ARGS... args) {
		BSSimpleList<T_Data>* pIter = const_cast<BSSimpleList<T_Data>*>(this);
		while (pIter) {
			func(pIter, args...);
			pIter = pIter->GetNext();
		}
	}

	[[nodiscard]] BSSimpleList<T_Data>* ReplaceAt(uint32_t auiIndex, const T_Data& aItem) {
		BSSimpleList<T_Data>* pIter = GetAt(auiIndex);
		BSSimpleList<T_Data>* pReplaced = nullptr;
		if (pIter) {
			pReplaced = new BSSimpleList<T_Data>(pIter->GetItem());
			pIter->SetItem(aItem);
		}
		return pReplaced;
	}

	[[nodiscard]] BSSimpleList<T_Data>* SetAt(uint32_t auiIndex, const T_Data& aItem) {
		BSSimpleList<T_Data>* pIter = GetAt(auiIndex);
		if (pIter)
			pIter->SetItem(aItem);
		else
			AddTail(aItem);
		return pIter;
	}

	BSSimpleList<T_Data>* AddAt(uint32_t auiIndex, const T_Data& aItem) {
		BSSimpleList<T_Data>* pIter = GetAt(auiIndex);
		if (pIter) {
			if (pIter->GetItem()) {
				BSSimpleList<T_Data>* pNewNode = new BSSimpleList<T_Data>(aItem);
				pNewNode->SetNext(pIter->GetNext());
				pIter->SetNext(pNewNode);
			}
			else {
				pIter->SetItem(aItem);
			}
		}
		else {
			AddTail(aItem);
		}
		return pIter;
	}

	BSSimpleList<T_Data>* RemoveAt(uint32_t auiIndex) {
		BSSimpleList<T_Data>* pIter = GetAt(auiIndex);
		if (pIter)
			Remove(pIter->GetItem());
		return pIter;
	}

	void InsertSorted(T_Data aItem, int32_t(__cdecl* apCompare)(const T_Data aItem1, const T_Data aItem2)) {
		if (!aItem)
			return;

		if (!this)
			return;

		BSSimpleList<T_Data>* pIter = this;
		BSSimpleList<T_Data>* pPrev = nullptr;
		bool bExit = false;

		while (true) {
			if (bExit)
				return;

			if (!pIter->GetItem())
				break;

			if (apCompare(aItem, pIter->GetItem()) <= 0) {
				if (pPrev)
					pPrev->SetNext(new BSSimpleList<T_Data>(aItem, pIter));
				else
					AddHead(aItem);
				goto EXIT_LOOP;
			}

			if (!pIter->GetNext()) {
				pIter->SetNext(new BSSimpleList<T_Data>(aItem));
				goto EXIT_LOOP;
			}

		NEXT:
			pPrev = pIter;
			pIter = pIter->GetNext();

			if (!pIter)
				return;
		}

		pIter->SetItem(aItem);

	EXIT_LOOP:
		bExit = true;
		goto NEXT;
	}
};

ASSERT_SIZE(BSSimpleList<uint32_t>, 0x8);