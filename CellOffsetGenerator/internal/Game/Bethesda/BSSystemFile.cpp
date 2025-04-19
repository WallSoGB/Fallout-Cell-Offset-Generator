#include "BSSystemFile.hpp"

// GAME - 0xB00900
BSSystemFile::BSSystemFile(const char* apName, AccessMode aeWriteType, OpenMode aeOpenMode, bool abSynchronousNoBuffer) {
	eState					= EC_NONE;
	uiWriteType				= aeWriteType;
	eOpenMode				= aeOpenMode;
	bSynchronousNoBuffer	= abSynchronousNoBuffer;
	eState					= DoOpen(apName);
}

// GAME - 0xB00950
BSSystemFile::~BSSystemFile() {
	ThisCall(0xB00950, this);
}

// GAME - 0xAFEAC0
uint32_t BSSystemFile::DoOpen(const char* apName) {
	return ThisCall<uint32_t>(0xAFEAC0, this, apName);
}

// GAME - 0xAFECD0
uint32_t BSSystemFile::DoRead(void* apBuffer, uint64_t aullBytes, uint32_t* auiRead) const {
	return ThisCall<uint32_t>(0xAFECD0, this, apBuffer, aullBytes, auiRead);
}

// GAME - 0xAFEDE0
uint32_t BSSystemFile::DoWrite(const void* apBuffer, uint64_t aullBytes, uint32_t* auiWrite) const {
	return ThisCall<uint32_t>(0xAFEDE0, this, apBuffer, aullBytes, auiWrite);
}

// GAME - 0xAFEEF0 
uint32_t BSSystemFile::DoSeek(uint64_t aullOffset, uint32_t aiWhence, uint64_t& arPos) const {
	return ThisCall<uint32_t>(0xAFEEF0, this, aullOffset, aiWhence, &arPos);
}

// GAME - 0xAFF000
uint32_t BSSystemFile::DoGetSize(uint64_t& arSize) const {
	return ThisCall<uint32_t>(0xAFF000, this, &arSize);
}

bool BSSystemFile::DoClose() const {
	return CloseHandle(hFile);
}

uint32_t BSSystemFile::DoFlush() {
	return FlushFileBuffers(hFile) ? EC_NONE : EC_FAILED;
}

uint32_t BSSystemFile::GetSizeOfFile(const char* apName, uint64_t& arSize) {
	BSSystemFile kFile = BSSystemFile(apName, AM_RDONLY, OM_NONE, false);
	uint32_t uiError = kFile.eState;
	if (uiError != EC_NONE)
		arSize = 0;
	else
		uiError = kFile.DoGetSize(arSize);

	return uiError;
}

// GAME - 0x8526D0
uint32_t BSSystemFile::Read(void* apBuffer, uint64_t aullBytes, uint32_t* auiRead) {
	eState = DoRead(apBuffer, aullBytes, auiRead);
	return eState;
}

// GAME - 0x856350
uint32_t BSSystemFile::Write(const void* apBuffer, uint64_t aullBytes, uint32_t* auiWrite) {
	eState = DoWrite(apBuffer, aullBytes, auiWrite);
	return eState;
}

// GAME - 0x8785E0
uint32_t BSSystemFile::Seek(uint64_t aullOffset, uint32_t aiWhence, uint64_t& arPos) {
	eState = DoSeek(aullOffset, aiWhence, arPos);
	return eState;
}

// GAME - 0x65C290
uint32_t BSSystemFile::GetSize(uint64_t& arSize) {
	eState = DoGetSize(arSize);
	return eState;
}
