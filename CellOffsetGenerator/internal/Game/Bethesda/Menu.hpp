#pragma once

#include "BSMemObject.hpp"
#include "BSSimpleList.hpp"
#include "BSString.hpp"

class TileMenu;
class Tile;
class TileExtra;

enum MenuSpecialKeyboardInputCode
{
	kMenu_Enter = -2,
	kMenu_UpArrow = 0x1,
	kMenu_DownArrow = 0x2,
	kMenu_RightArrow = 0x3,
	kMenu_LeftArrow = 0x4,
	kMenu_Space = 0x9,
	kMenu_Tab = 0xA,
	kMenu_ShiftEnter = 0xB,
	kMenu_AltEnter = 0xC,
	kMenu_ShiftLeft = 0xD,
	kMenu_ShiftRight = 0xE,
	kMenu_PageUp = 0xF,
	kMenu_PageDown = 0x10,
};

class Menu {
public:
	virtual				~Menu();
	virtual void		AttachTileByID(uint32_t auiTileID, Tile* apTile);
	virtual void		HandleLeftClickPress(uint32_t auiTileID, Tile* apCctiveTile); // called when the mouse has moved and left click is pressed
	virtual void		DoClick(int32_t auiTileID, Tile* apClickedTile);
	virtual void		HandleMouseover(uint32_t auiTileID, Tile* apActiveTile);
	virtual void		HandleUnmouseover(uint32_t auiTileID, Tile* apTile);
	virtual void		PostDragTileChange(uint32_t auiTileID, Tile* apNewTile, Tile* apActiveTile);
	virtual void		PreDragTileChange(uint32_t auiTileID, Tile* apOldTile, Tile* apActiveTile);
	virtual void		HandleActiveMenuClickHeld(uint32_t auiTileID, Tile* apActiveTile);
	virtual void		OnClickHeld(uint32_t auiTileID, Tile* apActiveTile);
	virtual void		HandleMousewheel(uint32_t auiTileID, Tile* apTile);
	virtual void		DoIdle();
	virtual bool		HandleKeyboardInput(uint32_t auiInputChar);
	virtual uint32_t	GetClass() const;
	virtual bool		HandleSpecialKeyInput(MenuSpecialKeyboardInputCode aeCode, float afKeyState);
	virtual bool		HandleControllerInput(int aiInput, Tile* apTile);
	virtual void		OnUpdateUserTrait(int aitileVal);
	virtual void		HandleControllerConnectOrDisconnect(bool abIsControllerConnected);

	struct TemplateData;

	TileMenu*							pRootTile;
	BSSimpleList<Menu::TemplateData*>	kMenuTemplates;
	Tile*								pLastTile;
	int32_t								iIsModal;
	int32_t								iMenuThickness;
	bool								bForceFadeOut;
	bool								bDeleteTemplates;
	uint32_t							uiID;
	uint32_t							uiVisibilityState;
};

ASSERT_SIZE(Menu, 0x28);