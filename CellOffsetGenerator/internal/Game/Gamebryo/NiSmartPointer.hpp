#pragma once

#include "NiMemObject.hpp"

template <class T_Data>
class NiPointer : public NiMemObject {
public:
	NiPointer();
	NiPointer(T_Data* apObject);
	NiPointer(const NiPointer& ptr);
	~NiPointer();

	T_Data* m_pObject;

	operator T_Data* () const;
	T_Data& operator*() const;
	T_Data* operator->() const;

	__forceinline NiPointer<T_Data>& operator =(const NiPointer& ptr) {
		if (m_pObject != ptr.m_pObject) {
			if (m_pObject)
				m_pObject->DecRefCount();
			m_pObject = ptr.m_pObject;
			if (m_pObject)
				m_pObject->IncRefCount();
		}
		return *this;
	}

	__forceinline NiPointer<T_Data>& operator =(T_Data* pObject) {
		if (m_pObject != pObject) {
			if (m_pObject)
				m_pObject->DecRefCount();
			m_pObject = pObject;
			if (m_pObject)
				m_pObject->IncRefCount();
		}
		return *this;
	}

	__forceinline bool operator==(T_Data* apObject) const { return (m_pObject == apObject); }

	__forceinline bool operator==(const NiPointer& ptr) const { return (m_pObject == ptr.m_pObject); }

	__forceinline operator bool() const { return m_pObject != nullptr; }
};

#define NiSmartPointer(className) \
    class className; \
    typedef NiPointer<className> className##Ptr;


template<class T_Data>
inline NiPointer<T_Data>::NiPointer() {
	m_pObject = nullptr;
}

template <class T_Data>
inline NiPointer<T_Data>::NiPointer(T_Data* apObject) {
	m_pObject = apObject;
	if (m_pObject)
		m_pObject->IncRefCount();
}

template <class T_Data>
inline NiPointer<T_Data>::NiPointer(const NiPointer& ptr) {
	m_pObject = ptr.m_pObject;
	if (m_pObject)
		m_pObject->IncRefCount();
}

template <class T_Data>
inline NiPointer<T_Data>::~NiPointer() {
	if (m_pObject)
		m_pObject->DecRefCount();
}

template <class T_Data>
inline NiPointer<T_Data>::operator T_Data* () const {
	return m_pObject;
}

template <class T_Data>
inline T_Data& NiPointer<T_Data>::operator*() const {
	return *m_pObject;
}

template <class T_Data>
inline T_Data* NiPointer<T_Data>::operator->() const {
	return m_pObject;
}