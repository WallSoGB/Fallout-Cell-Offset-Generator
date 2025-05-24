#pragma once

#include "TESForm.hpp"

class TESFile;

class TESObjectCELL : public TESForm {
public:
	TESObjectCELL();
	~TESObjectCELL();

	struct ExteriorData {
		int32_t		iCellX;
		int32_t		iCellY;
		Bitfield8	ucLandHideFlags;
	};

	struct InteriorData {
		uint32_t	uiAmbient;
		uint32_t	uiDirectional;
		uint32_t	uiFogColorNear;
		float		fFogNear;
		float		fFogFar;
		int32_t		iDirectionalXY;
		int32_t		iDirectionalZ;
		float		fDirectionalFade;
		float		fClipDist;
		float		fFogPower;
		uint32_t	uiInteriorOffset;
	};

	static uint32_t GetCoord(int16_t x, int16_t y);
	static uint32_t CalcExtGroupBlockKey(int32_t aiX, int32_t aiY);
	static uint32_t CalcExtGroupSubBlockKey(int32_t aiX, int32_t aiY);
};