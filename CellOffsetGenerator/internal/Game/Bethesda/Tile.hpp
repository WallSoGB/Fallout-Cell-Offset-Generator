#pragma once

#include "BSSimpleArray.hpp"
#include "BSSimpleList.hpp"
#include "BSString.hpp"
#include "NiColorA.hpp"
#include "NiSmartPointer.hpp"
#include "NiTList.hpp"

class NiNode;
class TileShaderProperty;

class Tile {
public:
	Tile();

	enum eTileValue {
		kTileValue_x = 0x0FA1,
		kTileValue_y,
		kTileValue_visible,
		kTileValue_class,
		kTileValue_clipwindow = 0x0FA6,
		kTileValue_stackingtype,
		kTileValue_locus,
		kTileValue_alpha,
		kTileValue_id,
		kTileValue_disablefade,
		kTileValue_listindex,
		kTileValue_depth,
		kTileValue_clips,
		kTileValue_target,
		kTileValue_height,
		kTileValue_width,
		kTileValue_red,
		kTileValue_green,
		kTileValue_blue,
		kTileValue_tile,
		kTileValue_childcount,
		kTileValue_child_count = kTileValue_childcount,
		kTileValue_justify,
		kTileValue_zoom,
		kTileValue_font,
		kTileValue_wrapwidth,
		kTileValue_wraplimit,
		kTileValue_wraplines,
		kTileValue_pagenum,
		kTileValue_ishtml,
		kTileValue_cropoffsety,
		kTileValue_cropy = kTileValue_cropoffsety,
		kTileValue_cropoffsetx,
		kTileValue_cropx = kTileValue_cropoffsetx,
		kTileValue_menufade,
		kTileValue_explorefade,
		kTileValue_mouseover,
		kTileValue_string,
		kTileValue_shiftclicked,
		kTileValue_clicked = 0x0FC7,
		kTileValue_clicksound = 0x0FCB,
		kTileValue_filename,
		kTileValue_filewidth,
		kTileValue_fileheight,
		kTileValue_repeatvertical,
		kTileValue_repeathorizontal,
		kTileValue_animation = 0x0FD2,
		kTileValue_linecount = 0x0DD4,
		kTileValue_pagecount,
		kTileValue_xdefault,
		kTileValue_xup,
		kTileValue_xdown,
		kTileValue_xleft,
		kTileValue_xright,
		kTileValue_xbuttona = 0x0FDD,
		kTileValue_xbuttonb,
		kTileValue_xbuttonx,
		kTileValue_xbuttony,
		kTileValue_xbuttonlt,
		kTileValue_xbuttonrt,
		kTileValue_xbuttonlb,
		kTileValue_xbuttonrb,
		kTileValue_xbuttonstart = 0x0FE7,
		kTileValue_mouseoversound,
		kTileValue_draggable,
		kTileValue_dragstartx,
		kTileValue_dragstarty,
		kTileValue_dragoffsetx,
		kTileValue_dragoffsety,
		kTileValue_dragdeltax,
		kTileValue_dragdeltay,
		kTileValue_dragx,
		kTileValue_dragy,
		kTileValue_wheelable,
		kTileValue_wheelmoved,
		kTileValue_systemcolor,
		kTileValue_brightness,
		kTileValue_linegap = 0x0FF7,
		kTileValue_resolutionconverter,
		kTileValue_texatlas,
		kTileValue_rotateangle,
		kTileValue_rotateaxisx,
		kTileValue_rotateaxisy,

		kTileValue_user0 = 0x01004,
		kTileValue_user1,
		kTileValue_user2,
		kTileValue_user3,
		kTileValue_user4,
		kTileValue_user5,
		kTileValue_user6,
		kTileValue_user7,
		kTileValue_user8,
		kTileValue_user9,
		kTileValue_user10,
		kTileValue_user11,
		kTileValue_user12,
		kTileValue_user13,
		kTileValue_user14,
		kTileValue_user15,
		kTileValue_user16,

		kTileValue_max
	};

	enum eTileID {
		kTileID_rect = 0x0385,
		kTileID_image,
		kTileID_text,
		kTileID_3D,
		kTileID_nif = kTileID_3D,
		kTileID_menu,

		// Not a Tile descendend
		kTileID_hotrect,
		kTileID_window,
		// This one descend from TileImage
		kTileID_radial,

		kTileID_max
	};

	virtual						~Tile();
	virtual void				Init(Tile* parent, const char* name, Tile* replacedChild);
	virtual NiNode*				MakeNode();
	virtual uint32_t			GetType() const;
	virtual const char*			GetTypeName() const;
	virtual bool				Unk_05(uint32_t arg0, uint32_t arg1);
	virtual uint32_t			PostParse(int, float, char const*);
	virtual void				ForceTextureRelease();
	virtual TileShaderProperty*	GetShaderProperty() const;
	virtual void				UpdateColor(NiNode* apNode, float afAlpha, NiColorA& arColor);

	struct Value;

	enum ActionType
	{
		kAction_copy = 0x7D0,
		kAction_add,
		kAction_sub,
		kAction_mul,
		kAction_div,
		kAction_min,
		kAction_max,
		kAction_mod,
		kAction_floor,
		kAction_ceil,
		kAction_abs,
		kAction_round,
		kAction_gt,
		kAction_gte,
		kAction_eq,
		kAction_neq,
		kAction_lt,
		kAction_lte,
		kAction_and,
		kAction_or,
		kAction_not,
		kAction_onlyif,
		kAction_onlyifnot,
		kAction_ref,
		kAction_begin,
		kAction_end,
	};

	// 0C
	class Action {
	public:
		Action();
		~Action();

		virtual float	GetFloat();
		virtual Value*	GetValue();

		uint32_t	uiType;
		Action*		pNext;
	};

	// 10
	class RefValueAction : public Action {
	public:
		RefValueAction();
		~RefValueAction();

		Value* pTileValue;
	};

	// 10
	class FloatAction : public Action {
	public:
		FloatAction();
		~FloatAction();

		float	fValue;
	};

	// 14
	struct Value {
		uint32_t	uiID;	
		Tile*		pParent;
		float		fNum;	
		char*		pcText;	
		Action*		pAction;
	};

	NiTList<Tile*>				kChildren;
	BSSimpleArray<Value*>		kValues;
	BSString					strName;
	Tile*						pParent;
	NiPointer<NiNode>			spNiNode;
	Bitfield32					uiFlags;
	uint8_t						unk34;
	bool						bShouldPaintRed;

	void SetValue(uint32_t auiID, const char* apString, bool abPropagate);
	void SetValue(uint32_t auiID, float afValue, bool abPropagate);

	Tile* ReadFile(const char* apFilePath);

	void SetParent(Tile* apParent, Tile* apPrevious);

	void Release();

	static Tile* GetChildByName(Tile* apTile, const char* apName);

	static uint32_t TextToTrait(const char* apTraitName);
};

ASSERT_SIZE(Tile, 0x38);