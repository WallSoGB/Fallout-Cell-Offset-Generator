#include "TESFile.hpp"

void TESFile::Destroy(bool abDeallocate) {
	ThisCall(0x4601A0, this, abDeallocate);
}

// GAME - 0x4739B0
TESFile* TESFile::GetThreadSafeFile() {
	return ThisCall<TESFile*>(0x4739B0, this);
}

// GAME - 0x473C70
TESFile* TESFile::GetThreadSafeParent() const {
	TESFile* pThreadSafeFile = pThreadSafeParent;
	for (; pThreadSafeFile && pThreadSafeFile->pThreadSafeParent; pThreadSafeFile = pThreadSafeFile->pThreadSafeParent)
		;
	return pThreadSafeFile;
}

// GAME - 0x471F80
void TESFile::TESRewind(bool abGetForm) {
	ThisCall(0x471F80, this, abGetForm);
}

// GAME - 0x473830
void TESFile::CloseAllOpenGroups() {
	ThisCall(0x473830, this);
}

// GAME - 0x473960
void TESFile::FreeDecompressedFormBuffer() {
	ThisCall(0x473960, this);
}

// GAME - 0x4734D0
bool TESFile::FindForm(const TESForm* apForm) {
    return ThisCall<bool>(0x4734D0, this, apForm);
}

// GAME - 0x472150
bool TESFile::NextForm(bool abSkipIgnored) {
    return ThisCall<bool>(0x472150, this, abSkipIgnored);
}

// GAME - 0x473660
bool TESFile::NextGroup() {
	return ThisCall<bool>(0x473660, this);
}

// GAME - 0x472D30
bool TESFile::ReadChunkHeader() {
	return ThisCall<bool>(0x472D30, this);
}

// GAME - 0x4726B0
CHUNK_ID TESFile::GetTESChunk() {
	if (eCurrentChunkID || ReadChunkHeader())
		return eCurrentChunkID;

	return NO_CHUNK;
}

// GAME - 0x4726F0
bool TESFile::NextChunk() {
	return ThisCall<bool>(0x4726F0, this);
}

// GAME - 0x472000
void TESFile::TESRewindChunk() {
	ThisCall(0x472000, this);
}

// GAME - 0x4720D0
void TESFile::ClearCurrentChunk() {
	eCurrentChunkID		= NO_CHUNK;
	uiActualChunkSize	= 0;
}

// GAME - 0x462270
TESFile::FormInfo* TESFile::GetCurrentForm() {
	return &kCurrentForm;
}

// GAME - 0x472890
bool TESFile::GetChunkData(void* apBuffer, uint32_t auiSize) {
	return ThisCall<bool>(0x472890, this, apBuffer, auiSize);
}

// GAME - 0x4723A0
bool TESFile::SetOffset(uint32_t auiOffset) {
	return ThisCall<bool>(0x4723A0, this, auiOffset);
}

// GAME - 0x471C20
bool TESFile::IsMaster() const {
	return uiFlags.GetBit(MASTER);
}

// GAME - 0x471C50
void TESFile::SetMaster(bool abMaster) {
	uiFlags.SetBit(MASTER, abMaster);
}

bool TESFile::IsOptimized() const {
	return uiFlags.GetBit(OPTIMIZED);
}

void TESFile::SetOptimized(bool abOptimized) {
	uiFlags.SetBit(OPTIMIZED, abOptimized);
}