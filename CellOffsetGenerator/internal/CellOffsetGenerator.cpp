#include "CellOffsetGenerator.hpp"
#include "BSSystemFile.hpp"
#include "LoadingMenu.hpp"
#include "ScrapHeap.hpp"
#include "TESDataHandler.hpp"
#include "TESFile.hpp"
#include "TESWorldSpace.hpp"
#include "TileMenu.hpp"

#include "shared/Utils/AutoScrapHeapBuffer.hpp"

#include <algorithm>

#define XXH_INLINE_ALL
#define XXH_ENABLE_AUTOVECTORIZE
#include "external/xxhash/xxhash.h"

#if LOGGING
#define DEBUG_MSG(...) _MESSAGE(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif

static const char OFFSETS_DIR[] = "Data\\CellOffsets";

XXH64_hash_t CreateHashForFile(TESFile* apFile, XXH3_state_t* apHashState) {
	constexpr uint32_t uiBufferSize = 8192;

	XXH3_64bits_reset(apHashState);
	apFile->TESRewind(false);
	apFile->pFile->ChangeBufferSize(uiBufferSize);
	char buffer[uiBufferSize];
	size_t count;

	while ((count = apFile->pFile->Read(&buffer, uiBufferSize)) != 0) {
		XXH3_64bits_update(apHashState, buffer, count);
	}

	return XXH3_64bits_digest(apHashState);
}

XXH64_hash_t CreateHashForOffsets(XXH3_state_t* apHashState, uint32_t* const& apOffsets, uint32_t auiOffsetCount) {
	XXH3_64bits_reset(apHashState);
	XXH3_64bits_update(apHashState, apOffsets, auiOffsetCount * sizeof(uint32_t));
	return XXH3_64bits_digest(apHashState);
}

class CellOffsetFile : public BSSystemFile {
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

	CellOffsetFile(const char* apName, AccessMode aeWriteType, OpenMode aeOpenMode) : BSSystemFile(apName, aeWriteType, aeOpenMode, false) {};
	~CellOffsetFile() {};

	Header	kHeader;
	Data	kData;

	void FillData(const XXH64_hash_t& arFileHash, const uint32_t& arOffsetCount, uint32_t* const& apOffsets) {
		kHeader.ullFileHash = arFileHash;

		kData.uiOffsetCount = arOffsetCount;
		kData.pOffsets = const_cast<uint32_t*>(apOffsets);
	}

	ErrorCode LoadHeader(XXH64_hash_t& arFileHash) {
		if (eState)
			return UNKNOWN;

		uint64_t ullFileSize;
		if (FAILED(GetSize(ullFileSize)) || !ullFileSize) [[unlikely]]
			return EMPTY_FILE;

		uint32_t uiBytesRead = 0;
		if (FAILED(Read(&kHeader, sizeof(kHeader), &uiBytesRead)))
			return UNKNOWN;

		if (uiBytesRead != sizeof(kHeader) || kHeader.uiHeaderID != MAGIC_NUMBER)
			return WRONG_FILE;

		if (kHeader.ullFileHash == arFileHash)
			return SUCCESS;
		else
			return HASH_MISMATCH;
	}

	ErrorCode LoadData(XXH3_state_t* apHashState) {
		uint32_t uiBytesRead = 0;
		Read(&kData.ullOffsetHash, sizeof(kData.ullOffsetHash), &uiBytesRead);
		Read(&kData.uiOffsetCount, sizeof(kData.uiOffsetCount), &uiBytesRead);

		if (kData.uiOffsetCount == UINT32_MAX) [[unlikely]]
			return EMPTY_WORLD;

		kData.pOffsets = BSNew<uint32_t>(kData.uiOffsetCount);
		Read(kData.pOffsets, kData.uiOffsetCount * sizeof(uint32_t), &uiBytesRead);

		if (uiBytesRead != kData.uiOffsetCount * sizeof(kData.uiOffsetCount)) [[unlikely]] {
			BSFree(kData.pOffsets);
			kData.pOffsets = nullptr;
			return READ_FAIL;
		}

		XXH64_hash_t ullCalculatedOffsetHash = CreateHashForOffsets(apHashState, kData.pOffsets, kData.uiOffsetCount);
		if (kData.ullOffsetHash != ullCalculatedOffsetHash) [[unlikely]] {
			BSFree(kData.pOffsets);
			kData.pOffsets = nullptr;
			return HASH_MISMATCH;
		}
		else {
			return SUCCESS;
		}
	}

	void SaveHeader() {
		if (eState)
			return;

		uint32_t uiWrittenBytes = 0;
		Write(&kHeader, sizeof(kHeader), &uiWrittenBytes);
	}

	void SaveData(XXH3_state_t* apHashState) {
		uint32_t uiWrittenBytes = 0;

		uint32_t uiOffsetDataSize = 0;
		if (kData.uiOffsetCount != UINT32_MAX) {
			uiOffsetDataSize = sizeof(uint32_t) * kData.uiOffsetCount;
			kData.ullOffsetHash = CreateHashForOffsets(apHashState, kData.pOffsets, kData.uiOffsetCount);
			Write(&kData.ullOffsetHash, sizeof(kData.ullOffsetHash), &uiWrittenBytes);
			Write(&kData.uiOffsetCount, sizeof(kData.uiOffsetCount), &uiWrittenBytes);
			Write(kData.pOffsets, uiOffsetDataSize, &uiWrittenBytes);
		}
		else {
			Write(&kData.ullOffsetHash, sizeof(kData.ullOffsetHash), &uiWrittenBytes);
			Write(&kData.uiOffsetCount, sizeof(kData.uiOffsetCount), &uiWrittenBytes);
		}

	}

	static bool ReadOffsets(const char* apFilePath, XXH64_hash_t aullHash, XXH3_state_t* apHashState, TESFile* apFile, TESWorldSpace* apWorld, TESWorldSpace::OFFSET_DATA* apOffsetData) {
		CellOffsetFile kFile(apFilePath, BSSystemFile::AM_RDONLY, BSSystemFile::OM_NONE);
		if (kFile.eState)
			return false;

		CellOffsetFile::ErrorCode eStatus = kFile.LoadHeader(aullHash);
		if (eStatus != CellOffsetFile::SUCCESS) {
			if (eStatus == CellOffsetFile::EMPTY_FILE)
				_MESSAGE("ReadOffsets - File %s is empty", apFilePath);
			else if (eStatus == CellOffsetFile::HASH_MISMATCH)
				_MESSAGE("ReadOffsets - Hash mismatch for %s, expected %016llX, got %016llX", apFilePath, aullHash, kFile.kHeader.ullFileHash);
			return false;
		}

		eStatus = kFile.LoadData(apHashState);
		if (eStatus == CellOffsetFile::SUCCESS) {
			apOffsetData->pCellFileOffsets = kFile.kData.pOffsets;
			return true;
		}

		if (eStatus == CellOffsetFile::EMPTY_WORLD) {
			_MESSAGE("ReadOffsets - World %s in file %s is empty", apWorld->GetFormEditorID(), apFile->GetName());
#if 0
			const int32_t iMaxX = int32_t(apOffsetData->kOffsetMaxCoords.x) >> 12;
			const int32_t iMaxY = int32_t(apOffsetData->kOffsetMaxCoords.y) >> 12;

			uint32_t uiMaxOffsetCount = apWorld->GetIndexForCellCoord(apFile, iMaxX, iMaxY);
			if (uiMaxOffsetCount == UINT32_MAX)
				DEBUG_MSG("ReadOffsets - Invalid coords for world %s in file %s", apWorld->GetFormEditorID(), apFile->GetName());
				return true;

			if (uiMaxOffsetCount == 0) [[unlikely]] {
				DEBUG_MSG("ReadOffsets - No offsets for %s", apWorld->GetFormEditorID());
				return true;
			}

			apOffsetData->pCellFileOffsets = BSNew<uint32_t>(uiMaxOffsetCount);
			if (apOffsetData->pCellFileOffsets)
				memset(apOffsetData->pCellFileOffsets, 0, sizeof(uint32_t) * uiMaxOffsetCount);
#endif

			return true;
		}
		else if (eStatus == CellOffsetFile::READ_FAIL) {
			_MESSAGE("ReadOffsets - Failed to read %i offsets from %s", kFile.kData.uiOffsetCount, apFilePath);
		}
		else if (eStatus == CellOffsetFile::HASH_MISMATCH) {
			_MESSAGE("ReadOffsets - Hash mismatch for %s! Plugin has changed cell data.", apFilePath);
		}

		return false;
	}

	static void SaveOffsets(const char* apFolderPath, const char* apFilePath, XXH64_hash_t aullHash, XXH3_state_t* apHashState, uint32_t* const& apOffsets, uint32_t auiOffsetCount) {
		CreateDirectory(apFolderPath, nullptr);
		CellOffsetFile kFile(apFilePath, BSSystemFile::AM_WRONLY, BSSystemFile::OM_CREAT);
		kFile.FillData(aullHash, auiOffsetCount, apOffsets);
		kFile.SaveHeader();
		kFile.SaveData(apHashState);
	}


};

OffsetGenerator::OffsetGenerator() {
	Start();
}

OffsetGenerator::~OffsetGenerator() {
	for (OffsetGenThread& kThread : kThreads)
		kThread.~OffsetGenThread();

	CloseHandle(hMainThread);
	hMainThread = nullptr;
}

uint32_t uiTotalWorlds = 0;
volatile uint32_t uiProcessedWorlds = 0;
volatile uint32_t uiProcessedFiles = 0;
uint32_t uiLastValue = 0;

void OffsetGenerator::RenderUI() {
	if (!bRunning)
		return;

	
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
		uint32_t uiProgressID = Tile::TextToTrait("_progress");
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

	bRunning = false;
}

uint32_t OffsetGenerator::GenerateCellOffsets(TESWorldSpace* apWorld, TESFile* apFile) {
	const char* pWorldName = apWorld->GetFormEditorID();
	auto pData = apWorld->GetOffsetData(apFile);
	if (!pData) {
		return 0;
	}

	if (pData->pCellFileOffsets) [[unlikely]] {
		DEBUG_MSG("GenerateCellOffsets - %s - Cell offsets already generated for %s", pWorldName, apFile->GetName());
		return 0;
	}

	const int32_t iMinX = int32_t(pData->kOffsetMinCoords.x) >> 12;
	const int32_t iMinY = int32_t(pData->kOffsetMinCoords.y) >> 12;
	const int32_t iMaxX = int32_t(pData->kOffsetMaxCoords.x) >> 12;
	const int32_t iMaxY = int32_t(pData->kOffsetMaxCoords.y) >> 12;

	// No cells
	if (iMinX == -524288) [[unlikely]] {
		return 0;
	}

	if (iMaxX - iMinX == -1 || iMaxY - iMinY == -1 || iMaxX - iMinX + 1 >= 1000 || iMaxY - iMinY + 1 >= 1000) [[unlikely]] {
		_MESSAGE("GenerateCellOffsets - File %s has invalid coords for %s", apFile->GetName(), pWorldName);
		return 0;
	}

	const uint32_t uiTotalCellCount = (iMaxX - iMinX) * (iMaxY - iMinY);

	uint32_t uiCellCount = 0;
	uint32_t uiOffsetCount = apWorld->GetIndexForCellCoord(apFile, iMaxX, iMaxY);
	if (!uiOffsetCount) [[unlikely]] {
		return 0;
	}
	uint32_t uiAllocSize = sizeof(uint32_t) * uiOffsetCount;

	AutoScrapHeapBuffer kHeap(uiAllocSize);
	memset(kHeap.pData, 0, uiAllocSize);

	for (int32_t y = iMinY; y <= iMaxY; y++) {
		for (int32_t x = iMinX; x <= iMaxX; x++) {
			TESWorldSpace::uiLastOffset = 0;
			uint32_t uiKey = apWorld->GetIndexForCellCoord(apFile, x, y);
			if (apWorld->FindCellInFile(apFile, x, y)) {
				uiCellCount++;
				reinterpret_cast<uint32_t*>(kHeap.pData)[uiKey] = TESWorldSpace::uiLastOffset - pData->uiFileOffset;
			}
		}
	}

	if (uiCellCount == 0) [[unlikely]] {
		DEBUG_MSG("GenerateCellOffsets - World %s in file %s is empty", apWorld->GetFormEditorID(), apFile->GetName());
		return UINT32_MAX;
	}

	pData->pCellFileOffsets = BSNew<uint32_t>(uiOffsetCount);
	memcpy(pData->pCellFileOffsets, kHeap.pData, uiAllocSize);

	return uiOffsetCount;
}

void OffsetGenerator::InitHooks() {
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

		// Remove other master checks
		// TESObjectCELL::Load
		PatchMemoryNop(0x54232A, 2);

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

DWORD WINAPI OffsetGenerator::ThreadProc(LPVOID lpThreadParameter) {
	OffsetGenerator* pGenerator = static_cast<OffsetGenerator*>(lpThreadParameter);
	std::vector<TESFile*> kFilesBySize;
	kFilesBySize.reserve(TESDataHandler::GetSingleton()->GetCompiledFileCount());

	for (TESWorldSpace* pWorld : TESDataHandler::GetSingleton()->kWorldSpaces) {
		for (TESFile* pFile : pWorld->kFiles) {
			TESWorldSpace::OFFSET_DATA* pData = pWorld->GetOffsetData(pFile);
			if (pData && pData->pCellFileOffsets)
				continue;

			kFilesBySize.push_back(pFile);
			uiTotalWorlds++;
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

	for (OffsetGenThread& kThread : pGenerator->kThreads)
		ResumeThread(kThread.hThread);

	WaitForMultipleObjects(pGenerator->kThreadHandles.size(), pGenerator->kThreadHandles.data(), TRUE, INFINITE);

	pGenerator->bDone = true;

	return 0;
}

void OffsetGenerator::Start() {
	if (hMainThread)
		return;

	CreateDirectory(OFFSETS_DIR, nullptr);

	SYSTEM_INFO kSysInfo;
	GetSystemInfo(&kSysInfo);
	const uint32_t uiCoreCount = std::min(DWORD(32), kSysInfo.dwNumberOfProcessors);
	const uint32_t uiLoadedFiles = TESDataHandler::GetSingleton()->kCompiledFiles.GetFileCount();

	__assume(uiCoreCount > 0 && uiLoadedFiles > 0);

	_MESSAGE("Initializing %i threads for %i files", uiCoreCount, uiLoadedFiles);
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

void CreateOffsetsForFile(TESFile* apFile, XXH3_state_t* apHashState) {
	XXH64_hash_t ullHash = CreateHashForFile(apFile, apHashState);

	char cFolderPath[MAX_PATH];
	sprintf_s(cFolderPath, "%s\\%s", OFFSETS_DIR, apFile->GetName());
	for (TESWorldSpace* pWorld : TESDataHandler::GetSingleton()->kWorldSpaces) {
		auto pOffsetData = pWorld->GetOffsetData(apFile);
		// Plugin doesn't contain the worldspace
		if (!pOffsetData)
			continue;

		// Plugin already has offsets for this worldspace
		if (pOffsetData->pCellFileOffsets) {
			uiProcessedWorlds++;
			continue;
		}

		char cFilePath[MAX_PATH];
		sprintf_s(cFilePath, "%s\\%s.fco", cFolderPath, pWorld->GetFormEditorID());

		if (CellOffsetFile::ReadOffsets(cFilePath, ullHash, apHashState, apFile, pWorld, pOffsetData)) {
			uiProcessedWorlds++;
			continue;
		}

		uint32_t uiOffsetCount = OffsetGenerator::GenerateCellOffsets(pWorld, apFile);
		if (!uiOffsetCount || !pOffsetData->pCellFileOffsets) {
			uiProcessedWorlds++;
			continue;
		}

		CellOffsetFile::SaveOffsets(cFolderPath, cFilePath, ullHash, apHashState, pOffsetData->pCellFileOffsets, uiOffsetCount);
		uiProcessedWorlds++;
	}
}

OffsetGenThread::OffsetGenThread() {
	hThread = nullptr;
}

OffsetGenThread::~OffsetGenThread() {
	if (hThread)
		CloseHandle(hThread);

	hThread = nullptr;
}

void OffsetGenThread::AddFile(const TESFile* apFile) {
	kFiles.push_back(const_cast<TESFile*>(apFile));
}

DWORD __stdcall OffsetGenThread::ThreadProc(LPVOID lpThreadParameter) {
	OffsetGenThread* pThread = (OffsetGenThread*)lpThreadParameter;

	pThread->Run();

	return 0;
}

bool OffsetGenThread::Run() {
	if (kFiles.empty())
		return false;

	uint32_t uiThread = GetCurrentThreadId();

	XXH3_state_t* pHashState = XXH3_createState();

	for (TESFile* pFile : kFiles) {
		TESFile* pThreadFile = pFile->GetThreadSafeFile();
		if (pThreadFile) {
			CreateOffsetsForFile(pThreadFile, pHashState);
		}

		uiProcessedFiles++;
	};

	XXH3_freeState(pHashState);

	for (TESFile* pFile : *TESDataHandler::GetSingleton()->GetFileList()) {
		TESFile* pThreadFile = nullptr;
		if (pFile->pThreadSafeFileMap && pFile->pThreadSafeFileMap->GetAt(uiThread, pThreadFile)) {
			pFile->pThreadSafeFileMap->RemoveAt(uiThread);

			// "Manual" CloseTES, since the original checks if TESDataHandler is closing files
			pThreadFile->CloseAllOpenGroups();
			delete pThreadFile->pFile;
			pThreadFile->pFile = nullptr;
			pThreadFile->FreeDecompressedFormBuffer();

			if (pThreadFile->pThreadSafeFileMap) {
				auto kIter = pThreadFile->pThreadSafeFileMap->GetFirstPos();
				while (kIter) {
					uint32_t uiThreadID;
					TESFile* pThreadFileChild = nullptr;
					pFile->pThreadSafeFileMap->GetNext(kIter, uiThreadID, pThreadFileChild);
				}
			}

			ThisCall(0x4601A0, pThreadFile, true); // TESFile destructor
		}
	}

	return false;
}