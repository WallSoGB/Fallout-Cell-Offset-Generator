#pragma once

#include "TESFile.hpp"
#include "TESObjectCELL.hpp"

#include <map>

namespace InteriorOffsets {

	typedef std::map<uint32_t, uint32_t> InteriorOffsets;
	std::map<TESFile*, InteriorOffsets*> kInteriorOffsetMap;
	thread_local TESFile* pCurrentFile = nullptr;

	InteriorOffsets* GetOffsetMapForFile(TESFile* apFile) {
		auto it = kInteriorOffsetMap.find(apFile);
		if (it != kInteriorOffsetMap.end()) [[likely]]
			return it->second;
		return nullptr;
	}

	void AddOffsetForFile(TESFile* apFile, uint32_t auiFormID, uint32_t auiOffset) {
		InteriorOffsets* pOffsets = GetOffsetMapForFile(apFile);
		if (pOffsets) [[likely]] {
			pOffsets->insert({ auiFormID, auiOffset });
		}
		else {
			InteriorOffsets* pOffsets = new InteriorOffsets;
			pOffsets->insert({ auiFormID, auiOffset });
			kInteriorOffsetMap.insert({ apFile, pOffsets });
		}
	}

	uint32_t GetOffsetForFile(TESFile* apFile, uint32_t auiFormID) {
		InteriorOffsets* pOffsets = GetOffsetMapForFile(apFile);
		if (pOffsets) [[likely]] {
			auto it = pOffsets->find(auiFormID);
			if (it != pOffsets->end()) [[likely]]
				return it->second;
		}
		return 0;
	}

	class TESFile : public ::TESFile {
	public:
		// Called when loading cell forms on game startup
		// Store currently loaded file
		uint32_t GetOffset() {
			pCurrentFile = this;
			return uiFileOffset;
		}

		// Called when loading cell data
		// Set offset if it exists in our map. Input is actually a form ID from TESObjectCELL::GetInteriorOffset below
		bool SetOffset(uint32_t auiOffset) {
			// ESP makes cell contents always loaded, and interior (compared to exterior) are also always loaded
			// So there's no situation where game ever looks for an interior cell, only its data
			// Thus, only master files have offsets
			if (!IsMaster()) 
				return false;

			bool bResult = false;
			const uint32_t uiNakedID = auiOffset;
			::TESFile* pFile = GetThreadSafeParent();
			if (!pFile) [[likely]]
				pFile = this;
			uint32_t uiActualOffset = GetOffsetForFile(pFile, uiNakedID);
			if (uiActualOffset) [[likely]]
				bResult = ::TESFile::SetOffset(uiActualOffset);
			return bResult;
		}
	};

	class TESObjectCELL : public ::TESObjectCELL {
	public:
		// Called when loading cell forms on game startup
		// Add interior offset to our maps
		void SetInteriorOffset(uint32_t auiOffset) {
			::TESFile* pFile = pCurrentFile->GetThreadSafeParent();
			if (!pFile) [[likely]]
				pFile = pCurrentFile;
			const uint32_t uiNakedID = GetFormID() & 0x00FFFFFF;
			AddOffsetForFile(pFile, uiNakedID, auiOffset);
			pCurrentFile = nullptr;

			// Original Code
			ThisCall(0x54DEF0, this, auiOffset);
		}

		// Called when loading cell data
		// Get form ID instead of the offset - we'll get the offset in TESFile::SetOffset above
		uint32_t GetInteriorOffset() {
			return GetFormID() & 0x00FFFFFF;
		}
	};

	void InitHooks() {
		// TESObjectCELL::Load
		ReplaceCallEx(0x54232F, &TESFile::GetOffset);
		ReplaceCallEx(0x542C1C, &TESObjectCELL::SetInteriorOffset);

		// TESObjectCELL::FindInFileFast
		ReplaceCallEx(0x5502B9, &TESObjectCELL::GetInteriorOffset);
		ReplaceCallEx(0x5502CE, &TESFile::SetOffset);
	}
}