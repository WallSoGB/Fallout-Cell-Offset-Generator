#pragma once

#include "NiPoint2.hpp"
#include "NiPoint3.hpp"
#include "NiSmartPointer.hpp"
#include "NiTMap.hpp"
#include "TESForm.hpp"
#include "TESFullName.hpp"
#include "TESObjectCELL.hpp"
#include "TESTexture.hpp"

class BSPortalGraph;
class BGSTerrainManager;
class TESClimate;
class TESImageSpace;
class TESWaterForm;
class BGSMusicType;
class BGSEncounterZone;
class BGSImpactData;
class TESGrassAreaParam;

class TESWorldSpace : public TESForm, public TESFullName, public TESTexture {
public:
	TESWorldSpace();
	virtual ~TESWorldSpace();
	virtual bool GetMapNameForLocation(BSString& arName, NiPoint3 akLocation);
	virtual void GetGrassForLocation(NiPoint2, NiPoint2, TESGrassAreaParam*, uint32_t);

	struct DCoordXY {
		int32_t	X;
		int32_t	Y;
	};

	struct WCoordXY {
		int16_t	X;
		int16_t	Y;
	};

	struct OFFSET_DATA {
		uint32_t*	pCellFileOffsets;
		NiPoint2	kOffsetMinCoords;
		NiPoint2	kOffsetMaxCoords;
		uint32_t	uiFileOffset;
	};

	struct MapData {
		DCoordXY	usableDimensions;
		WCoordXY	cellNWCoordinates;
		WCoordXY	cellSECoordinates;
	};	// 010

	struct ImpactData;

	enum Flags {
		SMALL_WORLD				= 1 << 0,
		NO_FAST_TRAVEL			= 1 << 1,
		UNK_2					= 1 << 2,
		UNK_3					= 1 << 3,
		NO_LOD_WATER			= 1 << 4,
		NO_LOD_NOISE			= 1 << 5,
		NO_NPC_FALL_DAMAGE		= 1 << 6,
		NEEDS_WATER_ADJUSTMENT	= 1 << 7,
	};

	typedef NiTPointerMap<uint32_t, BSSimpleList<TESObjectREFR*>*>	RefListPointerMap;
	typedef NiTPointerMap<int32_t, TESObjectCELL*>					CellPointerMap;
	typedef NiTMap<TESFile*, TESWorldSpace::OFFSET_DATA*>			OffsetDataMap;

	CellPointerMap*						pCellMap;
	TESObjectCELL*						pPersistentCell;
	uint32_t							kTerrainLODManager; // Unused
	BGSTerrainManager*					pTerrainManager;
	TESClimate*							pClimate;
	TESImageSpace*						pImageSpace;
	ImpactData*							pImpactSwap;
	Bitfield8							cFlags;
	Bitfield16							sParentUseFlags;
	RefListPointerMap					kFixedPersistentRefMap;
	BSSimpleList<TESObjectREFR*>		kMobilePersistentRefs;
	NiTMap<uint32_t, TESObjectREFR*>*	pOverlappedMultiBoundMap;
	NiPointer<BSPortalGraph>			spPortalGraph;
	TESWorldSpace*						pParentWorld;
	TESWaterForm*						pWorldWater;
	TESWaterForm*						pLODWater;
	float								fWaterLODHeight;
	MapData								kMapData;
	float								fWorldMapScale;
	float								fWorldMapCellX;
	float								fWorldMapCellY;
	BGSMusicType*						pMusic;
	NiPoint2							kMin;
	NiPoint2							kMax;
	OffsetDataMap						kOffsetMap;
	BSString							strEditorID;
	float								fDefaultLandHeight;
	float								fDefaultWaterHeight;
	BGSEncounterZone*					pEncounterZone;
	TESTexture							kCanopyShadow;
	TESTexture							kWaterNoiseTexture;

	TESObjectCELL* GetCellFromCellCoord(int32_t aiX, int32_t aiY) const;
	bool FindCellInFile(TESFile* apFile, int32_t aiX, int32_t aiY) const;
	uint32_t GetIndexForCellCoord(const TESFile* apFile, int32_t aiX, int32_t aiY) const;
	OFFSET_DATA* GetOffsetData(const TESFile* apFile) const;

	// Celloffset generator
	static thread_local uint32_t uiLastOffset;
	TESWorldSpace::OFFSET_DATA* GetOffsetDataSafe(const TESFile* apFile) const;
};

ASSERT_SIZE(TESWorldSpace, 0xEC);