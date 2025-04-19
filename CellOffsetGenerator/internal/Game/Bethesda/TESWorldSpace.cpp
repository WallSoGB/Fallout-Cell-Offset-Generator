#include "TESWorldSpace.hpp"
#include "TESFile.hpp"

#define ADD_OFFSET_PTR_CHECK 1

thread_local uint32_t TESWorldSpace::uiLastOffset = 0;

// GAME - 0x5875A0
// GECK - 0x6662D0
TESObjectCELL* TESWorldSpace::GetCellFromCellCoord(int32_t aiX, int32_t aiY) const {
    return ThisCall<TESObjectCELL*>(0x5875A0, this, aiX, aiY);
}

#define ALLOW_ESP

// GAME - 0x5854F0
// GECK - 0x6652B0
bool TESWorldSpace::FindCellInFile(TESFile* apFile, int32_t aiX, int32_t aiY) const {
	if (!apFile) [[unlikely]]
        return false;

    const OFFSET_DATA* pOffsetData = GetOffsetData(apFile);
#ifdef ALLOW_ESP
    if (!pOffsetData || !pOffsetData->pCellFileOffsets) {
#else
    if (!apFile->IsMaster() || !pOffsetData || !pOffsetData->pCellFileOffsets) {
#endif
        bool bResult = false;
        uint32_t uiBlockKey = TESObjectCELL::CalcExtGroupBlockKey(aiX, aiY);
        uint32_t uiSubBlockKey = TESObjectCELL::CalcExtGroupSubBlockKey(aiX, aiY);
        if (!apFile->FindForm(this))
            return bResult;

        TESObjectCELL::ExteriorData kExteriorData;

        apFile->NextForm(true);
        bool bReturn = false;
        bool bNextGroup = false;
        const TESFile::FormInfo* pForm = &apFile->kCurrentForm;
        while (true) {
            while (true) {
                if (!pForm || bReturn)
                    return bResult;

                if (pForm->uiRecordType != TESForm::GetFormEnumString(2)->uiFormString)
                    break;

                bNextGroup = true;
                bReturn = true;
                switch (pForm->uiFormID) {
                case 1:
                    bNextGroup = false;
                    bReturn = false;
                    break;
                case 4:
                    if (uiBlockKey == pForm->uiFormFlags.Get())
                        bNextGroup = false;
                    if (uiSubBlockKey == pForm->uiFormFlags.Get())
                        bNextGroup = false;
                    bReturn = false;
                    break;
                case 5:
                    if (uiSubBlockKey == pForm->uiFormFlags.Get())
                        bNextGroup = false;
                    bReturn = false;
                    break;
                case 6:
                case 8:
                case 9:
                    bReturn = false;
                    break;
                default:
                    break;
                }

                if (bReturn)
                    continue;

                if (bNextGroup)
                    apFile->NextGroup();
                else
                    apFile->NextForm(true);
            }
            if (pForm->uiRecordType == TESForm::GetFormEnumString(57)->uiFormString) {
                memset(&kExteriorData, 0, sizeof(TESObjectCELL::ExteriorData));
                if (pForm->uiFormFlags.GetBit(TESForm::PERSISTENT)) {
                    kExteriorData.iCellX = 0x7FFFFFFF;
                }
                else {
                    bool bChunkFound = false;
                    CHUNK_ID eChunk = apFile->GetTESChunk();
                    while (eChunk && !bChunkFound) {
                        if (eChunk == XCLC_ID) {
                            apFile->GetChunkData(&kExteriorData, sizeof(kExteriorData));
                            bChunkFound = true;
                        }

                        if (apFile->NextChunk())
                            eChunk = apFile->GetTESChunk();
                        else
                            eChunk = NO_CHUNK;
                    }
                }
                if (kExteriorData.iCellX == aiX && kExteriorData.iCellY == aiY) {
					uiLastOffset = apFile->uiFileOffset;
                    bResult = true;
                    bReturn = true;
                    apFile->TESRewindChunk();
                }
                else {
                LABEL_45:
                    apFile->NextForm(true);
                }
            }
            else {
                if (pForm->uiRecordType == TESForm::GetFormEnumString(68)->uiFormString)
                    goto LABEL_45;
                bReturn = true;
            }
        }
    }
    uint32_t uiCellIndex = GetIndexForCellCoord(apFile, aiX, aiY);
    if (uiCellIndex == UINT32_MAX)
        return false;
    
    uint32_t uiOffset = pOffsetData->pCellFileOffsets[uiCellIndex];
    if (!uiOffset)
        return false;

    apFile->SetOffset(pOffsetData->uiFileOffset + uiOffset);
    return true;
}

// GAME - 0x584530
uint32_t TESWorldSpace::GetIndexForCellCoord(const TESFile* apFile, int32_t aiX, int32_t aiY) const {
	OFFSET_DATA* pOffsetData = GetOffsetData(apFile);
    if (!pOffsetData)
		return UINT32_MAX;

	int32_t iMinX = int32_t(pOffsetData->kOffsetMinCoords.x) >> 12;
	int32_t iMinY = int32_t(pOffsetData->kOffsetMinCoords.y) >> 12;
	int32_t iMaxX = int32_t(pOffsetData->kOffsetMaxCoords.x) >> 12;
	int32_t iMaxY = int32_t(pOffsetData->kOffsetMaxCoords.y) >> 12;
	if (aiX <= iMaxX && aiX >= iMinX && aiY <= iMaxY && aiY >= iMinY)
        return aiX + (aiY - iMinY) * (iMaxX - iMinX + 1) - iMinX;
	else
		return UINT32_MAX;
}

// GAME - 0x588A90
TESWorldSpace::OFFSET_DATA* TESWorldSpace::GetOffsetData(const TESFile* apFile) const {
    return ThisCall<OFFSET_DATA*>(0x588A90, this, apFile);
}

// Made for GetExtCellDataFromFileByEditorID as it lacks a pCellFileOffsets check
TESWorldSpace::OFFSET_DATA* TESWorldSpace::GetOffsetDataSafe(const TESFile* apFile) const {
    OFFSET_DATA* pData = ThisCall<OFFSET_DATA*>(0x588A90, this, apFile);
    if (pData && pData->pCellFileOffsets)
        return pData;

    return nullptr;
}