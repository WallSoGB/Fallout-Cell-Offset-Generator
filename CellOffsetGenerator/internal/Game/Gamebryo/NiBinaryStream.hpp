#pragma once

class NiBinaryStream;
typedef uint32_t(__cdecl* NIBINARYSTREAM_READFN)(NiBinaryStream* apThis, void* apvBuffer, uint32_t auiBytes, uint32_t* apuiComponentSizes, uint32_t auiNumComponents);
typedef uint32_t(__cdecl* NIBINARYSTREAM_WRITEFN)(NiBinaryStream* apThis, const void* apvBuffer, uint32_t auiBytes, uint32_t* apuiComponentSizes, uint32_t auiNumComponents);

class NiBinaryStream {
public:
	NiBinaryStream();
	virtual ~NiBinaryStream() {};

	virtual 			operator bool() = 0;
	virtual void		Seek(int32_t aiNumBytes) = 0;
	virtual uint32_t	GetPosition() const;
	virtual void		SetEndianSwap(bool abDoSwap) = 0;

	uint32_t				m_uiAbsoluteCurrentPos;
	NIBINARYSTREAM_READFN	m_pfnRead;
	NIBINARYSTREAM_WRITEFN	m_pfnWrite;

	static void DoByteSwap(void* apvData, uint32_t auiBytes, uint32_t* apuiComponentSizes, uint32_t auiNumComponents);

	// GAME - 0x462D80
	__forceinline uint32_t Read(void* apvBuffer, uint32_t auiBytes) {
		uint32_t uiSize = 1;
		uint32_t uiBytesRead = BinaryRead(apvBuffer, auiBytes, &uiSize, 1);
		return uiBytesRead;
	}

	__forceinline uint32_t Write(const void* apvBuffer, uint32_t auiBytes) {
		uint32_t uiSize = 1;
		uint32_t uiBytesWritten = BinaryWrite(apvBuffer, auiBytes, &uiSize, 1);
		return uiBytesWritten;
	}

	// GAME - 0x462DC0
	__forceinline uint32_t BinaryRead(void* apvBuffer, uint32_t auiTotalBytes, uint32_t* apuiComponentSizes, uint32_t auiNumComponents) {
		uint32_t uiBytesRead = m_pfnRead(this, apvBuffer, auiTotalBytes, apuiComponentSizes, auiNumComponents);
		m_uiAbsoluteCurrentPos += uiBytesRead;
		return uiBytesRead;
	}

	__forceinline uint32_t BinaryWrite(const void* apvBuffer, uint32_t auiTotalBytes, uint32_t* apuiComponentSizes, uint32_t auiNumComponents) {
		uint32_t uiBytesWritten = m_pfnWrite(this, apvBuffer, auiTotalBytes, apuiComponentSizes, auiNumComponents);
		m_uiAbsoluteCurrentPos += uiBytesWritten;
		return uiBytesWritten;
	}

protected:
	friend class BetterBSA;
	uint32_t	GetPositionEx() const;
};

ASSERT_SIZE(NiBinaryStream, 0x10);

template <class T_Data>
void NiBinaryStreamLoad(NiBinaryStream& is, T_Data* pValue, uint32_t auiNumEls = 1) {
	uint32_t uiSize = sizeof(T_Data);
	is.BinaryRead(pValue, uiSize * auiNumEls, &uiSize, 1);
}

template <class T_Data>
void NiBinaryStreamSave(NiBinaryStream& os, const T_Data* pValue, uint32_t auiNumEls = 1) {
	uint32_t uiSize = sizeof(T_Data);
	os.BinaryWrite(pValue, uiSize * auiNumEls, &uiSize, 1);
}

template <class T_Data>
void NiStreamLoadBinary(NiBinaryStream& binstream, T_Data& value) {
	NiBinaryStreamLoad(binstream, &value, 1);
}

template <class T_Data>
void NiStreamLoadBinary(NiBinaryStream& binstream, T_Data* value, uint32_t auiNumEls) {
	NiBinaryStreamLoad(binstream, value, auiNumEls);
}

template <class T_Data>
void NiStreamSaveBinary(NiBinaryStream& binstream, const T_Data& value) {
	NiBinaryStreamSave(binstream, &value, 1);
}

template <class T_Data>
void NiStreamSaveBinary(NiBinaryStream& binstream, const T_Data* value, uint32_t auiNumEls) {
	NiBinaryStreamSave(binstream, value, auiNumEls);
}