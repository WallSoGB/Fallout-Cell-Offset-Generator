#pragma once

class BSSystemFile {
public:
    enum AccessMode {
        AM_RDONLY   = 0,
        AM_RDWR     = 1,
        AM_WRONLY   = 2
    };

    enum OpenMode {
        OM_NONE     = 0,
        OM_APPEND   = 1,
        OM_CREAT    = 2,
        OM_TRUNC    = 3,
        OM_EXCL     = 4
    };

    enum SeekMode {
        SM_SET = 0,
        SM_CUR = 1,
        SM_END = 2
    };

    enum ErrorCode {
        EC_NONE             = 0,
        EC_INVALID_FILE     = 1,
        EC_NOT_EXIST        = 2,
        EC_ALREADY_EXISTS   = 3,
        EC_FAILED           = 4
    };

    enum FileOperation {
        FO_OPEN     = 0,
        FO_READ     = 1,
        FO_WRITE    = 2,
        FO_SEEK     = 3
    };

	BSSystemFile(const char* apName, AccessMode aeWriteType, OpenMode aeOpenMode, bool abSynchronousNoBuffer);
    ~BSSystemFile();

	DWORD	eState;
	DWORD	uiWriteType;
	DWORD	eOpenMode;
	bool	bSynchronousNoBuffer;
	HANDLE	hFile;

protected:
	uint32_t DoOpen(const char* apName);
	uint32_t DoRead(void* apBuffer, uint64_t aullBytes, uint32_t* auiRead) const;
	uint32_t DoWrite(const void* apBuffer, uint64_t aullBytes, uint32_t* auiWrite) const;
	uint32_t DoSeek(uint64_t aiOffset, uint32_t aiWhence, uint64_t& arPos) const;
	uint32_t DoGetSize(uint64_t& arSize) const;
    bool     DoClose() const;
    uint32_t DoFlush();

public:
    static uint32_t GetSizeOfFile(const char* apName, uint64_t& arSize);

	uint32_t Read(void* apBuffer, uint64_t aullBytes, uint32_t* auiRead);
	uint32_t Write(const void* apBuffer, uint64_t aullBytes, uint32_t* auiWrite);
	uint32_t Seek(uint64_t aiOffset, uint32_t aiWhence, uint64_t& arPos);
	uint32_t GetSize(uint64_t& arSize);
};

ASSERT_SIZE(BSSystemFile, 0x14);