#pragma once

#include "NiBinaryStream.hpp"

class NiFile : public NiBinaryStream {
public:
	enum OpenMode {
		READ_ONLY	= 0,
		WRITE_ONLY	= 1,
		APPEND_ONLY = 2,
	};

	NiFile();
	NiFile(const char* apFileName, OpenMode aeMode, uint32_t auiBufferAllocSize);
	~NiFile() override;

	virtual void		SeekAlt(int32_t aiOffset, int32_t aiWhence);
	virtual const char*	GetFilename() const;
	virtual uint32_t	GetFileSize();

	uint32_t	m_uiBufferAllocSize;
	uint32_t	m_uiBufferReadSize;
	uint32_t	m_uiPos;
	uint32_t	m_uiAbsolutePos;
	char*		m_pBuffer;
	FILE*		m_pFile;
	OpenMode	m_eMode;
	bool		m_bGood;
};

ASSERT_SIZE(NiFile, 0x30);