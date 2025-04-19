#include "TESDataHandler.hpp"

TESDataHandler* TESDataHandler::GetSingleton() {
#ifdef GAME
    return *(TESDataHandler**)0x11C3F2C;
#else
	return *(TESDataHandler**)0xED3B0C;
#endif
}

// GAME - 0x45DFC0
BSSimpleList<TESFile*>* TESDataHandler::GetFileList() {
	return &kFiles;
}

uint32_t TESDataHandler::GetCompiledFileCount() const {
	return kCompiledFiles.GetFileCount();
}

// GAME - 0x465010
// GECK - 0x4CDFA0
TESFile* TESDataHandler::GetCompiledFile(uint32_t auiIndex) const {
	if (auiIndex <= 0xFE)
		return kCompiledFiles.GetFile(auiIndex);
	else
		return nullptr;
}

uint32_t CompiledFiles::GetFileCount() const {
	return uiCompiledFileCount;
}

TESFile* CompiledFiles::GetFile(uint32_t auiIndex) const {
	ASSUME_ASSERT(auiIndex < 0xFF);
	return pFileArray[auiIndex];
}