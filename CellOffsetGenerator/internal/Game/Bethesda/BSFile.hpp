#pragma once

#include "NiFile.hpp"
#include "BSString.hpp"

class BSFile : public NiFile {
public:
	BSFile();
	BSFile(const char* apName, OpenMode aeMode, uint32_t auiBufferSize, bool abTextMode);
	~BSFile() override;

	virtual bool		Open(int = 0, bool abTextMode = false);
	virtual bool		OpenByFilePointer(FILE* apFile);
	virtual uint32_t	GetSize();
	virtual uint32_t	ReadString(BSString& arString, uint32_t auiMaxLength);
	virtual uint32_t	ReadStringAlt(BSString& arString, uint32_t auiMaxLength);
	virtual uint32_t	GetLine(char* apBuffer, uint32_t auiMaxBytes, uint8_t aucMark);
	virtual uint32_t	WriteString(BSString& arString, bool abBinary);
	virtual uint32_t	WriteStringAlt(BSString& arString, bool abBinary);
	virtual bool		IsReadable();
	virtual uint32_t	DoRead(void* apBuffer, uint32_t auiBytes);
	virtual uint32_t	DoWrite(const void* apBuffer, uint32_t auiBytes);

	bool		bUseAuxBuffer;
	void*		pAuxBuffer;
	int32_t		iAuxTrueFilePos;
	int32_t		iAuxBufferMinIndex;
	int32_t		iAuxBufferMaxIndex;
	char		cFileName[260];
	uint32_t	uiResult;
	uint32_t	uiIOSize;
	uint32_t	uiTrueFilePos;
	uint32_t	uiFileSize;

	bool ChangeBufferSize(uint32_t auiSize);
};

ASSERT_SIZE(BSFile, 0x158);