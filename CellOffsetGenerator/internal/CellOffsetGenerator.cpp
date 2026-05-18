#include "CellOffsetGenerator.hpp"
#include "LoadingMenu.hpp"
#include "ScrapHeap.hpp"
#include "TESDataHandler.hpp"
#include "TESFile.hpp"
#include "TESWorldSpace.hpp"
#include "TileMenu.hpp"

#include "shared/Utils/AutoScrapHeapBuffer.hpp"

#include <algorithm>
#include <atomic>

#define XXH_INLINE_ALL
#define XXH_ENABLE_AUTOVECTORIZE
#include "external/xxhash/xxhash.h"

#if LOGGING
#define DEBUG_MSG(...) _MESSAGE(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif

static const char OFFSETS_DIR[] = "Data\\CellOffsets";
volatile bool		bGeneratingOffsets = false;
volatile uint32_t	uiTotalWorlds = 0;
volatile uint32_t	uiProcessedWorlds = 0;
uint32_t uiLastValue = 0;

static XXH64_hash_t __fastcall CreateHashForFile(TESFile* apFile, XXH3_state_t* apHashState) noexcept {
	XXH3_64bits_reset(apHashState);
	
	HANDLE hFileMapping = CreateFileMappingA(apFile->pFile->m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (hFileMapping) [[likely]] {
		void* pBuffer = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
		if (!pBuffer) [[unlikely]] {
			CloseHandle(hFileMapping);
			_MESSAGE("Failed to map view of file for %s, error code: %u", apFile->GetName(), GetLastError());
			DebugBreak();
			return 0;
		}
		XXH3_64bits_update(apHashState, pBuffer, apFile->uiFileSize);
		UnmapViewOfFile(pBuffer);
		CloseHandle(hFileMapping);
	}
	else {
		_MESSAGE("Failed to create file mapping for %s, error code: %u", apFile->GetName(), GetLastError());
		DebugBreak();
		return 0;
	}

	return XXH3_64bits_digest(apHashState);
}

static XXH64_hash_t __fastcall CreateHashForOffsets(XXH3_state_t* apHashState, uint32_t* const& apOffsets, uint32_t auiOffsetCount) noexcept {
	XXH3_64bits_reset(apHashState);
	XXH3_64bits_update(apHashState, apOffsets, auiOffsetCount * sizeof(uint32_t));
	return XXH3_64bits_digest(apHashState);
}

static void __fastcall CreateEmptyOffset(TESWorldSpace::OFFSET_DATA* apOffsetData) noexcept {
	apOffsetData->pCellFileOffsets = BSMemory::malloc<uint32_t>();
	apOffsetData->pCellFileOffsets[0] = 0;

	apOffsetData->kOffsetMaxCoords = 0.f;
	apOffsetData->kOffsetMinCoords = 0.f;
}

class CellOffsetFile {
public:
	static constexpr uint32_t MAGIC_NUMBER = 'FCOF';
	static constexpr uint32_t FILE_VERSION = 1;

	enum ErrorCode : uint32_t {
		SUCCESS			= 0,
		EMPTY_FILE,
		WRONG_FILE,
		HASH_MISMATCH,
		EMPTY_WORLD,
		READ_FAIL,
		VERSION_MISMATCH,
		CELL_COUNT_CHANGE,
		UNKNOWN			= UINT32_MAX,
	};

	struct Header {
		uint32_t		uiHeaderID	= MAGIC_NUMBER;
		uint32_t		uiVersion	= FILE_VERSION;
		XXH64_hash_t	ullFileHash = 0;
	};

	struct Data {
		XXH64_hash_t	ullOffsetHash	= 0;
		uint32_t		uiOffsetCount	= 0;
		uint32_t*		pOffsets		= 0;
	};

	CellOffsetFile(const char* apName, uint32_t aeAccessMode, uint32_t aeOpenMode) noexcept {
		hFile = CreateFile(apName, aeAccessMode, FILE_SHARE_READ, nullptr, aeOpenMode, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	};
	~CellOffsetFile() noexcept {
		if (IsValid())
			CloseHandle(hFile);
	};

	HANDLE		hFile;
	Header		kHeader;
	Data		kData;

private:

	BOOL __fastcall Read(void* apBuffer, uint32_t auiSize, uint32_t& aruiBytesRead) noexcept {
		return ReadFile(hFile, apBuffer, auiSize, reinterpret_cast<LPDWORD>(&aruiBytesRead), nullptr);
	}

	BOOL __fastcall Write(const void* apBuffer, uint32_t auiSize, uint32_t& aruiBytesWritten) noexcept {
		return WriteFile(hFile, apBuffer, auiSize, reinterpret_cast<LPDWORD>(&aruiBytesWritten), nullptr);
	}

	BOOL __fastcall GetFileSize(uint64_t& arullFileSize) const noexcept {
		uint64_t ullFileSize = 0;
		if (!GetFileSizeEx(hFile, reinterpret_cast<PLARGE_INTEGER>(&ullFileSize)))
			return FALSE;
		arullFileSize = ullFileSize;
		return TRUE;
	}

	BOOL __fastcall IsValid() const noexcept {
		return hFile != INVALID_HANDLE_VALUE;
	}

public:

	void __fastcall FillData(const XXH64_hash_t& arFileHash, const uint32_t& arOffsetCount, uint32_t* const& apOffsets) noexcept {
		kHeader.ullFileHash = arFileHash;

		kData.uiOffsetCount = arOffsetCount;
		kData.pOffsets = const_cast<uint32_t*>(apOffsets);
	}

	ErrorCode __fastcall LoadHeader(XXH64_hash_t& arFileHash) noexcept {
		uint64_t ullFileSize = 0;
		if (!GetFileSize(ullFileSize) || !ullFileSize) [[unlikely]]
			return EMPTY_FILE;

		uint32_t uiBytesRead = 0;
		if (!Read(&kHeader, sizeof(kHeader), uiBytesRead))
			return UNKNOWN;

		if (uiBytesRead != sizeof(kHeader) || kHeader.uiHeaderID != MAGIC_NUMBER)
			return WRONG_FILE;

		if (kHeader.uiVersion != FILE_VERSION)
			return VERSION_MISMATCH;

		if (kHeader.ullFileHash == arFileHash)
			return SUCCESS;
		else
			return HASH_MISMATCH;
	}

	ErrorCode __fastcall LoadData(XXH3_state_t* apHashState, uint32_t auiCurrentCellCount) noexcept {
		uint32_t uiBytesRead = 0;
		Read(&kData.ullOffsetHash, sizeof(kData.ullOffsetHash), uiBytesRead);
		Read(&kData.uiOffsetCount, sizeof(kData.uiOffsetCount), uiBytesRead);

		if (kData.uiOffsetCount == UINT32_MAX) [[unlikely]]
			return EMPTY_WORLD;

		if (kData.uiOffsetCount != auiCurrentCellCount)
			return CELL_COUNT_CHANGE;

		const uint32_t uiOffsetBytesCount = kData.uiOffsetCount * sizeof(uint32_t);
		kData.pOffsets = BSMemory::malloc<uint32_t>(kData.uiOffsetCount);
		Read(kData.pOffsets, uiOffsetBytesCount, uiBytesRead);

		if (uiBytesRead != uiOffsetBytesCount) [[unlikely]] {
			BSMemory::free(kData.pOffsets);
			kData.pOffsets = nullptr;
			return READ_FAIL;
		}

		XXH64_hash_t ullCalculatedOffsetHash = CreateHashForOffsets(apHashState, kData.pOffsets, kData.uiOffsetCount);
		if (kData.ullOffsetHash != ullCalculatedOffsetHash) [[unlikely]] {
			BSMemory::free(kData.pOffsets);
			kData.pOffsets = nullptr;
			return HASH_MISMATCH;
		}
		else {
			return SUCCESS;
		}
	}

	void __fastcall SaveHeader() noexcept {
		uint32_t uiBytesWritten = 0;
		Write(&kHeader, sizeof(kHeader), uiBytesWritten);
	}

	void __fastcall SaveData(XXH3_state_t* apHashState) noexcept {
		uint32_t uiBytesWritten = 0;

		uint32_t uiOffsetDataSize = 0;
		if (kData.uiOffsetCount != UINT32_MAX) {
			uiOffsetDataSize = sizeof(uint32_t) * kData.uiOffsetCount;
			kData.ullOffsetHash = CreateHashForOffsets(apHashState, kData.pOffsets, kData.uiOffsetCount);
			Write(&kData.ullOffsetHash, sizeof(kData.ullOffsetHash), uiBytesWritten);
			Write(&kData.uiOffsetCount, sizeof(kData.uiOffsetCount), uiBytesWritten);
			Write(kData.pOffsets, uiOffsetDataSize, uiBytesWritten);
		}
		else {
			Write(&kData.ullOffsetHash, sizeof(kData.ullOffsetHash), uiBytesWritten);
			Write(&kData.uiOffsetCount, sizeof(kData.uiOffsetCount), uiBytesWritten);
		}
	}

	static bool __fastcall ReadOffsets(const char* apFilePath, XXH64_hash_t aullHash, XXH3_state_t* apHashState, TESFile* apFile, TESWorldSpace* apWorld, TESWorldSpace::OFFSET_DATA* apOffsetData) noexcept {
		CellOffsetFile kFile(apFilePath, GENERIC_READ, OPEN_EXISTING);
		if (!kFile.IsValid())
			return false;

		CellOffsetFile::ErrorCode eStatus = kFile.LoadHeader(aullHash);
		if (eStatus != CellOffsetFile::SUCCESS) {
			if (eStatus == CellOffsetFile::EMPTY_FILE)
				_MESSAGE("ReadOffsets - File %s is empty", apFilePath);
			else if (eStatus == CellOffsetFile::HASH_MISMATCH)
				_MESSAGE("ReadOffsets - Hash mismatch for %s, expected %016llX, got %016llX", apFilePath, aullHash, kFile.kHeader.ullFileHash);
			else if (eStatus == CellOffsetFile::VERSION_MISMATCH) {
				if (kFile.kHeader.uiVersion > FILE_VERSION)
					_MESSAGE("ReadOffsets - File has newer version (%i) than supported (%i)", kFile.kHeader.uiVersion, FILE_VERSION);
				else
					_MESSAGE("ReadOffsets - File has older version (%i), upgrading to %i", kFile.kHeader.uiVersion, FILE_VERSION);
			}
			return false;
		}

		const int32_t iMinX = int32_t(apOffsetData->kOffsetMinCoords.x) >> 12;
		const int32_t iMinY = int32_t(apOffsetData->kOffsetMinCoords.y) >> 12;
		const int32_t iMaxX = int32_t(apOffsetData->kOffsetMaxCoords.x) >> 12;
		const int32_t iMaxY = int32_t(apOffsetData->kOffsetMaxCoords.y) >> 12;

		const uint32_t uiTotalCellCount = (iMaxX - iMinX + 1) * (iMaxY - iMinY + 1);

		eStatus = kFile.LoadData(apHashState, uiTotalCellCount);
		if (eStatus == CellOffsetFile::SUCCESS) {
			apOffsetData->pCellFileOffsets = kFile.kData.pOffsets;
			return true;
		}
		else if (eStatus == CellOffsetFile::EMPTY_WORLD) {
			DEBUG_MSG("ReadOffsets - World %s in file %s is empty", apWorld->GetFormEditorID(), apFile->GetName());

			CreateEmptyOffset(apOffsetData);

			return true;
		}
		else if (eStatus == CellOffsetFile::CELL_COUNT_CHANGE) {
			_MESSAGE("ReadOffsets - World %s offset array size mismatch (%i calculated, %i saved)", apWorld->GetFormEditorID(), uiTotalCellCount, kFile.kData.uiOffsetCount);
		}
		else if (eStatus == CellOffsetFile::READ_FAIL) {
			_MESSAGE("ReadOffsets - Failed to read %i offsets from %s", kFile.kData.uiOffsetCount, apFilePath);
		}
		else if (eStatus == CellOffsetFile::HASH_MISMATCH) {
			_MESSAGE("ReadOffsets - Hash mismatch for %s! Plugin has changed cell data.", apFilePath);
		}

		return false;
	}

	static void __fastcall SaveOffsets(const char* apFolderPath, const char* apFilePath, XXH64_hash_t aullHash, XXH3_state_t* apHashState, uint32_t* const& apOffsets, uint32_t auiOffsetCount) noexcept {
		CreateDirectory(apFolderPath, nullptr);
		CellOffsetFile kFile(apFilePath, GENERIC_WRITE, CREATE_ALWAYS);
		if (kFile.IsValid()) {
			kFile.FillData(aullHash, auiOffsetCount, apOffsets);
			kFile.SaveHeader();
			kFile.SaveData(apHashState);
		}
		else {
			_MESSAGE("SaveOffsets - Failed to create file %s", apFilePath);
		}
	}


};

OffsetGenerator::OffsetGenerator() noexcept {
	Start();
}

OffsetGenerator::~OffsetGenerator() noexcept {
	for (OffsetGenThread& kThread : kThreads)
		kThread.~OffsetGenThread();

	CloseHandle(hMainThread);
	hMainThread = nullptr;

	bGeneratingOffsets  = false;
	uiTotalWorlds		= 0;
	uiProcessedWorlds	= 0;
	uiLastValue			= 0;

	// Release memory we allocated on threads
	ScrapHeapManager::GetSingleton()->FreeAllBuffers();
}

void OffsetGenerator::RenderUI() noexcept {
	if (!bRunning)
		return;

	// Wait until we actually have something to generate
	// That way we don't show the progress bar if we are not generating anything
	while (!bDone && !bGeneratingOffsets) {
		Sleep(1);
	}

	if (!bDone) {
		Menu* pLoadingMenu = LoadingMenu::GetSingleton();
		Tile* pUI = nullptr;
		Tile* pProgressText = nullptr;

		bool bFirstRun = false;
		if (!pUI) {
			pUI = pLoadingMenu->pRootTile->ReadFile("data\\menus\\prefabs\\OffsetGen\\ProgressMeter.xml");
			pProgressText = Tile::GetChildByName(pUI, "ProgressText");
			bFirstRun = true;
		}

		if (pUI) {
			const uint32_t uiProgressID = Tile::TextToTrait("_progress");
			while (!bDone) {
				if (uiLastValue != uiProcessedWorlds || bFirstRun) {
					float fProgress = float(uiProcessedWorlds) / uiTotalWorlds;
					pUI->SetValue(uiProgressID, fProgress, false);

					char cBuffer[64];
					sprintf_s(cBuffer, "%i/%i", uiProcessedWorlds, uiTotalWorlds);
					pProgressText->SetValue(Tile::kTileValue_string, cBuffer, true);

					uiLastValue = uiProcessedWorlds;
				}
			}
			pUI->Release();
			delete pUI;
		}
	}

	bRunning = false;
}

uint32_t __fastcall OffsetGenerator::GenerateCellOffsets(TESWorldSpace* apWorld, TESFile* apFile, ScrapHeap* apHeap) noexcept {
	auto pData = apWorld->GetOffsetData(apFile);
	if (!pData) {
		return 0;
	}

	if (pData->pCellFileOffsets) [[unlikely]] {
		DEBUG_MSG("GenerateCellOffsets - %s - Cell offsets already generated for %s", apWorld->GetFormEditorID(), apFile->GetName());
		return 0;
	}

	const int32_t iMinX = int32_t(pData->kOffsetMinCoords.x) >> 12;
	const int32_t iMinY = int32_t(pData->kOffsetMinCoords.y) >> 12;
	const int32_t iMaxX = int32_t(pData->kOffsetMaxCoords.x) >> 12;
	const int32_t iMaxY = int32_t(pData->kOffsetMaxCoords.y) >> 12;

	const uint32_t uiTotalCellCount = (iMaxX - iMinX + 1) * (iMaxY - iMinY + 1);

	// No cells
	if (iMinX == -524288 || uiTotalCellCount <= 1) [[unlikely]] {
		DEBUG_MSG("GenerateCellOffsets - World %s in file %s reports as empty", apWorld->GetFormEditorID(), apFile->GetName());
		return UINT32_MAX;
	}

	if (iMaxX - iMinX == -1 || iMaxY - iMinY == -1 || iMaxX - iMinX + 1 >= 1000 || iMaxY - iMinY + 1 >= 1000) [[unlikely]] {
		_MESSAGE("GenerateCellOffsets - File %s has invalid coords for %s", apFile->GetName(), apWorld->GetFormEditorID());
		return 0;
	}

	uint32_t uiCellCount = 0;
	const uint32_t uiAllocSize = sizeof(uint32_t) * uiTotalCellCount;

	AutoScrapHeapBuffer kHeap(uiAllocSize, alignof(uint32_t), apHeap);
	memset(kHeap.pData, 0, uiAllocSize);

	DEBUG_MSG("GenerateCellOffsets - Scanning file %s for world %s", apFile->GetName(), apWorld->GetFormEditorID());
	for (int32_t y = iMinY; y <= iMaxY; y++) {
		for (int32_t x = iMinX; x <= iMaxX; x++) {
			TESWorldSpace::uiLastOffset = 0;
			uint32_t uiKey = apWorld->GetIndexForCellCoord(apFile, x, y);
			if (uiKey != UINT32_MAX && apWorld->FindCellInFile(apFile, x, y)) {
				++uiCellCount;
				reinterpret_cast<uint32_t*>(kHeap.pData)[uiKey] = TESWorldSpace::uiLastOffset - pData->uiFileOffset;
			}
		}
	}

	if (uiCellCount == 0) [[unlikely]] {
		DEBUG_MSG("GenerateCellOffsets - World %s in file %s is empty", apWorld->GetFormEditorID(), apFile->GetName());
		CreateEmptyOffset(pData);
		return UINT32_MAX;
	}

	bGeneratingOffsets = true;

	pData->pCellFileOffsets = BSMemory::malloc<uint32_t>(uiTotalCellCount);
	memcpy(pData->pCellFileOffsets, kHeap.pData, uiAllocSize);

	return uiTotalCellCount;
}

void OffsetGenerator::InitHooks() noexcept {
	// Remove master file checks
	{
		// TESWorldSpace::Load
		PatchMemoryNop(0x5835BC, 2);
		PatchMemoryNop(0x583DC0, 2);
		PatchMemoryNop(0x583EA8, 2);

		// TESWorldSpace::LoadPartial
		PatchMemoryNop(0x58344E, 2);
		PatchMemoryNop(0x58351B, 2);

		// TESWorldSpace::FindInFileFast
		PatchMemoryNop(0x5858DC, 2);

		// TESWorldSpace::FindCellInFile
		PatchMemoryNop(0x585502, 12);

		// TESObjectCELL::FindInFileFast
		PatchMemoryNop(0x5502A1, 6);
	}

	// Remove optimized file checks
	{
		// TESDataHandler::GetExtCellDataFromFileByEditorID
		PatchMemoryNop(0x461DB1, 2);
	}

	// Nullcheck fix
	{
		ReplaceCallEx(0x58598F, &TESWorldSpace::GetOffsetDataSafe);
	}
}

DWORD WINAPI OffsetGenerator::ThreadProc(LPVOID lpThreadParameter) noexcept {
	OffsetGenerator* pGenerator = static_cast<OffsetGenerator*>(lpThreadParameter);
	{
		std::vector<TESFile*> kFilesBySize;
		kFilesBySize.reserve(TESDataHandler::GetSingleton()->GetCompiledFileCount());

		for (TESWorldSpace* pWorld : TESDataHandler::GetSingleton()->kWorldSpaces) {
			for (TESFile* pFile : pWorld->kFiles) {
				TESWorldSpace::OFFSET_DATA* pData = pWorld->GetOffsetData(pFile);
				if (pData && pData->pCellFileOffsets)
					continue;

				kFilesBySize.push_back(pFile);
				++uiTotalWorlds;
			}
		}

		if (uiTotalWorlds == 0) {
			_MESSAGE("All worlds have cell offsets already! Skipping generation");
			pGenerator->bDone = true;
			return 0;
		}

		// Remove duplicates
		std::sort(kFilesBySize.begin(), kFilesBySize.end());
		auto kEnd = std::unique(kFilesBySize.begin(), kFilesBySize.end());
		kFilesBySize.erase(kEnd, kFilesBySize.end());

		// Sort files by size, from largest to smallest
		std::sort(kFilesBySize.begin(), kFilesBySize.end(), [](TESFile* apA, TESFile* apB) {
			return apA->uiFileSize > apB->uiFileSize;
			});

		// Distribute files across threads
		uint32_t uiThreadIndex = 0;
		for (TESFile* pFile : kFilesBySize) {
			pGenerator->kThreads[uiThreadIndex].AddFile(pFile);
			uiThreadIndex = (uiThreadIndex + 1) % pGenerator->kThreads.size();
		}
	}

	for (OffsetGenThread& kThread : pGenerator->kThreads)
		ResumeThread(kThread.hThread);

	WaitForMultipleObjects(pGenerator->kThreadHandles.size(), pGenerator->kThreadHandles.data(), TRUE, INFINITE);

	for (OffsetGenThread& kThread : pGenerator->kThreads) {
		const DWORD dwThreadID = GetThreadId(kThread.hThread);

		for (TESFile* pFile : kThread.kFiles) {
			TESFile* pThreadFile = nullptr;
			if (pFile->pThreadSafeFileMap && pFile->pThreadSafeFileMap->GetAt(dwThreadID, pThreadFile)) {
				pFile->pThreadSafeFileMap->RemoveAt(dwThreadID);

				// "Manual" CloseTES, since the original checks if TESDataHandler is closing files
				pThreadFile->CloseAllOpenGroups();
				delete pThreadFile->pFile;
				pThreadFile->pFile = nullptr;
				pThreadFile->FreeDecompressedFormBuffer();

				ThisCall(0x4601A0, pThreadFile, true); // TESFile destructor
			}
		}
	}

	pGenerator->bDone = true;

	return 0;
}

void OffsetGenerator::Start() noexcept {
	if (hMainThread)
		return;

	CreateDirectory(OFFSETS_DIR, nullptr);

	SYSTEM_INFO kSysInfo;
	GetSystemInfo(&kSysInfo);
	const uint32_t uiCoreCount = std::min(DWORD(32), kSysInfo.dwNumberOfProcessors);

	__assume(uiCoreCount > 0);

	_MESSAGE("Initializing %i threads", uiCoreCount);
	kThreads.resize(uiCoreCount);

	for (uint32_t i = 0; i < uiCoreCount; i++) {
		OffsetGenThread& kThread = kThreads[i];
		kThread.hThread = CreateThread(nullptr, 0x10000, OffsetGenThread::ThreadProc, &kThread, CREATE_SUSPENDED, nullptr);
		if (kThread.hThread) {
			kThreadHandles.push_back(kThread.hThread);
			wchar_t cThreadName[64];
			_snwprintf_s(cThreadName, sizeof(cThreadName), L"Cell Offset Generator Thread %i", i);
			SetThreadDescription(kThread.hThread, cThreadName);
		}
		else {
			_MESSAGE("Failed to create thread %i", i);
		}
	}

	hMainThread = CreateThread(nullptr, 0x10000, ThreadProc, this, 0, nullptr);
	bRunning = true;
}

void __fastcall CreateOffsetsForFile(TESFile* apFile, XXH3_state_t* apHashState, ScrapHeap* apHeap) noexcept {
	const XXH64_hash_t ullHash = CreateHashForFile(apFile, apHashState);

	char cFolderPath[MAX_PATH];
	char cFilePath[MAX_PATH];
	sprintf_s(cFolderPath, "%s\\%s", OFFSETS_DIR, apFile->GetName());
	for (TESWorldSpace* pWorld : TESDataHandler::GetSingleton()->kWorldSpaces) {
		auto pOffsetData = pWorld->GetOffsetData(apFile);
		// Plugin doesn't contain the worldspace
		if (!pOffsetData)
			continue;

		// Plugin already has offsets for this worldspace
		if (pOffsetData->pCellFileOffsets) {
			++uiProcessedWorlds;
			continue;
		}

		sprintf_s(cFilePath, "%s\\%s.fco", cFolderPath, pWorld->GetFormEditorID());

		if (CellOffsetFile::ReadOffsets(cFilePath, ullHash, apHashState, apFile, pWorld, pOffsetData)) {
			++uiProcessedWorlds;
			continue;
		}

		const uint32_t uiOffsetCount = OffsetGenerator::GenerateCellOffsets(pWorld, apFile, apHeap);
		if (!uiOffsetCount || (!pOffsetData->pCellFileOffsets && uiOffsetCount != UINT32_MAX)) {
			++uiProcessedWorlds;
			continue;
		}

		CellOffsetFile::SaveOffsets(cFolderPath, cFilePath, ullHash, apHashState, pOffsetData->pCellFileOffsets, uiOffsetCount);
		++uiProcessedWorlds;
	}
}

OffsetGenThread::OffsetGenThread() noexcept {
	hThread = nullptr;
}

OffsetGenThread::~OffsetGenThread() noexcept {
	if (hThread)
		CloseHandle(hThread);

	hThread = nullptr;
}

void OffsetGenThread::AddFile(const TESFile* apFile) noexcept {
	kFiles.push_back(const_cast<TESFile*>(apFile));
}

DWORD __stdcall OffsetGenThread::ThreadProc(LPVOID lpThreadParameter) noexcept {
	OffsetGenThread* pThread = static_cast<OffsetGenThread*>(lpThreadParameter);

	pThread->Run();

	return 0;
}

bool OffsetGenThread::Run() noexcept {
	if (kFiles.empty())
		return false;

	XXH3_state_t* pHashState = XXH3_createState();

	ScrapHeap kHeap;
	for (TESFile* pFile : kFiles) {
		TESFile* pThreadFile = pFile->GetThreadSafeFile();
		if (pThreadFile)
			CreateOffsetsForFile(pThreadFile, pHashState, &kHeap);
	};

	XXH3_freeState(pHashState);

	return false;
}