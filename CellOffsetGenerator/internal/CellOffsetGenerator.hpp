#pragma once

#include <vector>

class Tile;
class TESFile;
class TESWorldSpace;

class OffsetGenThread {
public:
	OffsetGenThread();
	~OffsetGenThread();

	void AddFile(const TESFile* apFile);

protected:
	friend class OffsetGenerator;

	HANDLE					hThread = nullptr;
	std::vector<TESFile*>	kFiles;

	static DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);

	bool Run();
};

class OffsetGenerator {
public:
	OffsetGenerator();
	~OffsetGenerator();

	void Start();

	void RenderUI();

	static uint32_t GenerateCellOffsets(TESWorldSpace* apWorld, TESFile* apFile);

	static void InitHooks();

private:
	bool						 bRunning		= false;
	volatile bool				 bDone			= false;
	HANDLE						 hMainThread	= nullptr;
	std::vector<HANDLE>			 kThreadHandles;
	std::vector<OffsetGenThread> kThreads;

	static DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);
};