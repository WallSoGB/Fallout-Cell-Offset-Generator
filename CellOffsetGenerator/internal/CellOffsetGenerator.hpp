#pragma once

#include <vector>
#include "Game/Bethesda/ScrapHeap.hpp"

class Tile;
class TESFile;
class TESWorldSpace;

class OffsetGenThread {
public:
	OffsetGenThread() noexcept;
	~OffsetGenThread() noexcept;

	void AddFile(const TESFile* apFile) noexcept;

protected:
	friend class OffsetGenerator;

	HANDLE					hThread = nullptr;
	std::vector<TESFile*>	kFiles;

	static DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) noexcept;

	bool Run() noexcept;
};

class OffsetGenerator {
public:
	OffsetGenerator() noexcept;
	~OffsetGenerator() noexcept;

	void Start() noexcept;

	void RenderUI() noexcept;

	static uint32_t __fastcall GenerateCellOffsets(TESWorldSpace* apWorld, TESFile* apFile, ScrapHeap* apHeap = nullptr) noexcept;

	static void InitHooks() noexcept;

private:
	bool						 bRunning		= false;
	volatile bool				 bDone			= false;
	HANDLE						 hMainThread	= nullptr;
	std::vector<HANDLE>			 kThreadHandles;
	std::vector<OffsetGenThread> kThreads;

	static DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) noexcept;
};