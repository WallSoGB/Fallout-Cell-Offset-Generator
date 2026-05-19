#pragma once

#include "TESFile.hpp"
#include "TESObjectCELL.hpp"

#include <unordered_map>

namespace InteriorOffsets {

	typedef std::unordered_map<uint32_t, uint32_t> InteriorOffsets;
	std::unordered_map<const TESFile*, InteriorOffsets*>* pInteriorOffsetMap;
	thread_local const TESFile* pCurrentFile = nullptr;

	InteriorOffsets* __fastcall GetOffsetMapForFile(const TESFile* apFile) noexcept {
		auto it = pInteriorOffsetMap->find(apFile);
		if (it != pInteriorOffsetMap->end()) [[likely]]
			return it->second;
		return nullptr;
	}

	void __fastcall AddOffsetForFile(const TESFile* apFile, uint32_t auiFormID, uint32_t auiOffset) noexcept {
		InteriorOffsets* pOffsets = GetOffsetMapForFile(apFile);
		if (pOffsets) [[likely]] {
			pOffsets->insert({ auiFormID, auiOffset });
		}
		else {
			InteriorOffsets* pOffsets = new InteriorOffsets;
			pOffsets->insert({ auiFormID, auiOffset });
			pInteriorOffsetMap->insert({ apFile, pOffsets });
		}
	}

	uint32_t __fastcall GetOffsetForFile(const TESFile* apFile, uint32_t auiFormID) noexcept {
		InteriorOffsets* pOffsets = GetOffsetMapForFile(apFile);
		if (pOffsets) [[likely]] {
			auto it = pOffsets->find(auiFormID);
			if (it != pOffsets->end()) [[likely]]
				return it->second;
		}
		return 0;
	}

	class TESFileEx : public TESFile {
	public:
		// Called when loading cell forms on game startup
		// Store currently loaded file
		uint32_t GetOffset() noexcept {
			pCurrentFile = this;
			return uiFileOffset;
		}

		// Called when loading cell data
		// Set offset if it exists in our map. Input is actually a form ID from TESObjectCELL::GetInteriorOffset below
		bool SetOffset(uint32_t auiOffset) noexcept {
			// ESP makes cell contents always loaded, and interior (compared to exterior) cells are also always loaded
			// So there's no situation where game ever looks for an interior cell, only its data
			// Thus, only master files have offsets
			if (!IsMaster()) 
				return false;

			bool bResult = false;
			const TESFile* pFile = GetThreadSafeParent();
			if (!pFile) [[likely]]
				pFile = this;
			uint32_t uiActualOffset = GetOffsetForFile(pFile, auiOffset);
			if (uiActualOffset) [[likely]]
				bResult = TESFile::SetOffset(uiActualOffset);
			return bResult;
		}
	};

	class TESObjectCELLEx : public ::TESObjectCELL {
	public:
		// Called when loading cell forms on game startup
		// Add interior offset to our maps
		void SetInteriorOffset(uint32_t auiOffset) noexcept {
			const TESFile* pFile = pCurrentFile->GetThreadSafeParent();
			if (!pFile) [[likely]]
				pFile = pCurrentFile;

			AddOffsetForFile(pFile, GetFormID(), auiOffset);
			pCurrentFile = nullptr;

			// Original Code
			ThisCall(0x54DEF0, this, auiOffset);
		}

		// Called when loading cell data
		// Get form ID instead of the offset - we'll get the offset in TESFile::SetOffset above
		uint32_t GetInteriorOffset() noexcept {
			return GetFormID();
		}
	};

	void InitHooks() noexcept {
		pInteriorOffsetMap = new std::unordered_map<const TESFile*, InteriorOffsets*>;

		// TESObjectCELL::Load
		ReplaceCallEx(0x54232F, &TESFileEx::GetOffset);
		ReplaceCallEx(0x542C1C, &TESObjectCELLEx::SetInteriorOffset);

		// TESObjectCELL::FindInFileFast
		ReplaceCallEx(0x5502B9, &TESObjectCELLEx::GetInteriorOffset);
		ReplaceCallEx(0x5502CE, &TESFileEx::SetOffset);
	}
}