#pragma once

#include "BaseFormComponent.hpp"
#include "NiTPointerMap.hpp"
#include "BSSimpleList.hpp"
#include "BSString.hpp"

#define IS_ID(form, type) (form->ucFormType == kFormType_##type)
#define NOT_ID(form, type) (form->ucFormType != kFormType_##type)

class TESFile;
class NiColor;
class BGSSaveGameBuffer;
class BGSLoadFormBuffer;
class TESObjectREFR;
class TESBoundObject;
class Script;

struct FORM_ENUM_STRING {
	uint8_t			ucFormID;
	const char*		pFormString;
	uint32_t		uiFormString;
};

struct FORM {
	uint32_t uiRecordType;
	uint32_t uiDataSize;
	uint32_t uiFormID;
	uint32_t uiFormFlags;
	uint32_t uiVersionControl;
	uint16_t usFormVersion;
	uint16_t usVCVersion;
};

struct FORM_GROUP {
	FORM		kGroupData;
	uint32_t	uiGroupOffset;
};


enum FormType {
	kFormType_None = 0,					// 00
	kFormType_TES4,
	kFormType_Group,
	kFormType_GMST,
	kFormType_BGSTextureSet,
	kFormType_BGSMenuIcon,
	kFormType_TESGlobal,
	kFormType_TESClass,
	kFormType_TESFaction,					// 08
	kFormType_BGSHeadPart,
	kFormType_TESHair,
	kFormType_TESEyes,
	kFormType_TESRace,
	kFormType_TESSound,
	kFormType_BGSAcousticSpace,
	kFormType_TESSkill,
	kFormType_EffectSetting,					// 10
	kFormType_Script,
	kFormType_TESLandTexture,
	kFormType_EnchantmentItem,
	kFormType_SpellItem,
	kFormType_TESObjectACTI,
	kFormType_BGSTalkingActivator,
	kFormType_BGSTerminal,
	kFormType_TESObjectARMO,					// 18	inv object
	kFormType_TESObjectBOOK,						// 19	inv object
	kFormType_TESObjectCLOT,					// 1A	inv object
	kFormType_TESObjectCONT,
	kFormType_TESObjectDOOR,
	kFormType_IngredientItem,				// 1D	inv object
	kFormType_TESObjectLIGH,					// 1E	inv object
	kFormType_TESObjectMISC,						// 1F	inv object
	kFormType_TESObjectSTAT,					// 20
	kFormType_BGSStaticCollection,
	kFormType_BGSMovableStatic,
	kFormType_BGSPlaceableWater,
	kFormType_TESGrass,
	kFormType_TESObjectTREE,
	kFormType_TESFlora,
	kFormType_TESFurniture,
	kFormType_TESObjectWEAP,					// 28	inv object
	kFormType_TESAmmo,						// 29	inv object
	kFormType_TESNPC,						// 2A
	kFormType_TESCreature,					// 2B
	kFormType_TESLevCreature,			// 2C
	kFormType_TESLevCharacter,			// 2D
	kFormType_TESKey,						// 2E	inv object
	kFormType_AlchemyItem,				// 2F	inv object
	kFormType_BGSIdleMarker,				// 30
	kFormType_BGSNote,						// 31	inv object
	kFormType_BGSConstructibleObject,		// 32	inv object
	kFormType_BGSProjectile,
	kFormType_TESLevItem,				// 34	inv object
	kFormType_TESWeather,
	kFormType_TESClimate,
	kFormType_TESRegion,
	kFormType_NavMeshInfoMap,						// 38
	kFormType_TESObjectCELL,
	kFormType_TESObjectREFR,				// 3A
	kFormType_Character,						// 3B
	kFormType_Creature,						// 3C
	kFormType_MissileProjectile,						// 3D
	kFormType_GrenadeProjectile,						// 3E
	kFormType_BeamProjectile,						// 3F
	kFormType_FlameProjectile,						// 40
	kFormType_TESWorldSpace,
	kFormType_TESObjectLAND,
	kFormType_NavMesh,
	kFormType_TLOD,
	kFormType_TESTopic,
	kFormType_TESTopicInfo,
	kFormType_TESQuest,
	kFormType_TESIdleForm,						// 48
	kFormType_TESPackage,
	kFormType_TESCombatStyle,
	kFormType_TESLoadScreen,
	kFormType_TESLevSpell,
	kFormType_TESObjectANIO,
	kFormType_TESWaterForm,
	kFormType_TESEffectShader,
	kFormType_TOFT,						// 50	table of Offset (see OffsetData in Worldspace)
	kFormType_BGSExplosion,
	kFormType_BGSDebris,
	kFormType_TESImageSpace,
	kFormType_TESImageSpaceModifier,
	kFormType_BGSListForm,					// 55
	kFormType_BGSPerk,
	kFormType_BGSBodyPartData,
	kFormType_BGSAddonNode,				// 58
	kFormType_ActorValueInfo,
	kFormType_BGSRadiationStage,
	kFormType_BGSCameraShot,
	kFormType_BGSCameraPath,
	kFormType_BGSVoiceType,
	kFormType_BGSImpactData,
	kFormType_BGSImpactDataSet,
	kFormType_TESObjectARMA,						// 60
	kFormType_BGSEncounterZone,
	kFormType_BGSMessage,
	kFormType_BGSRagdoll,
	kFormType_DOBJ,
	kFormType_BGSLightingTemplate,
	kFormType_BGSMusicType,
	kFormType_TESObjectIMOD,					// 67	inv object
	kFormType_TESReputation,				// 68
	kFormType_ContinuousBeamProjectile,						// 69 Continuous Beam
	kFormType_TESRecipe,
	kFormType_TESRecipeCategory,
	kFormType_TESCasinoChips,				// 6C	inv object
	kFormType_TESCasino,
	kFormType_TESLoadScreenType,
	kFormType_MediaSet,
	kFormType_MediaLocationController,	// 70
	kFormType_TESChallenge,
	kFormType_TESAmmoEffect,
	kFormType_TESCaravanCard,				// 73	inv object
	kFormType_TESCaravanMoney,				// 74	inv object
	kFormType_TESCaravanDeck,
	kFormType_BGSDehydrationStage,
	kFormType_BGSHungerStage,
	kFormType_BGSSleepDeprevationStage,	// 78
	kFormType_Count
};

class TESForm : public BaseFormComponent {
public:
	TESForm();
	
	virtual					~TESForm();
	virtual void			InitializeData();
	virtual void			ClearData();
	virtual bool			Check(); // NavMesh returns false, rest 1, NavMeshInfoMap does shit
	virtual bool			Load(TESFile* apFile);
	virtual bool			LoadPartial(TESFile* apFile);
	virtual bool			Save(TESFile* apFile);
	virtual void			SaveAlt();					// Writes data
	virtual bool			LoadEdit(TESFile* apFile);
	virtual bool			SaveEdit(TESFile* apFile);
	virtual bool			SavesBefore(FORM* apForm) const;
	virtual bool			SavesBeforeAlt(TESForm* apForm) const;
	virtual TESForm*		CreateDuplicateForm(bool, NiTPointerMap<TESForm*, TESForm*>*);
	virtual void			PostDuplicateProcess(NiTPointerMap<TESForm*, TESForm*>*);
	virtual void			AddChange(uint32_t auiChangedFlags);
	virtual void			RemoveChange(uint32_t auiChangedFlags);
	virtual uint32_t		GetSaveSize(uint32_t auiChangedFlags) const;
	virtual void			SaveGameBGS(BGSSaveGameBuffer* apBuffer);
	virtual void			SaveGameTES(uint32_t auiChangedFlags);
	virtual void			LoadGameBGS(BGSSaveGameBuffer* apBuffer);
	virtual void			LoadGameTES(uint32_t auiChangedFlags, uint32_t);
	virtual void			InitLoadGameBGS(BGSSaveGameBuffer* apBuffer);
	virtual void			InitLoadGameTES(uint32_t, uint32_t);
	virtual void			FinishInitLoadGameTES(uint32_t, uint32_t);
	virtual void			RevertBGS(BGSLoadFormBuffer* apBuffer);
	virtual void			RevertTES(uint32_t auiChangedFlags);
	virtual void			Unk_1E(void* arg);
	virtual bool			FindInFileFast(TESFile* apFile);
	virtual void			CheckSaveGame(BGSLoadFormBuffer* apBuffer);
	virtual void			FinishLoadGameBGS(BGSLoadFormBuffer* apBuffer);
	virtual void			InitItem();
	virtual uint32_t		GetSavedFormType() const;
	virtual void			GetFormDetailedString(BSString& arDest) const;
	virtual bool			GetQuestObject() const;
	virtual bool			HasTalkedToPC() const;
	virtual bool			GetHavokDeath() const;
	virtual bool			GetRandomAnim() const;
	virtual bool			IsNeedToChangeProcess() const;
	virtual bool			Unk_2A();
	virtual bool			HasSpecificTextures() const; // Has Platform/Language Specific Textures
	virtual bool			GetObstacleFlag() const;
	virtual bool			GetFlag40000000() const;
	virtual bool			GetOnLocalMap() const;
	virtual void			SetCastsShadows(bool abShadowCaster);
	virtual NiColor*		GetEmittanceColor();
	virtual void			SetDelete(bool abDeleted);
	virtual void			SetAltered(bool abAltered);
	virtual void			SetQuestObject(bool abQuestObject);
	virtual void			SetTalkedToPC(bool abTalkedTo);
	virtual void			SetHavokDeath(bool abDied);
	virtual void			SetNeedToChangeProcess(bool abChange);
	virtual void			SaveObjectBound();
	virtual void			LoadObjectBound(TESFile* apFile);
	virtual bool			IsBoundObject() const;
	virtual bool			IsObject() const;
	virtual bool			IsMagicItem() const;
	virtual bool			IsReference() const;
	virtual bool			IsArmorAddon() const;
	virtual bool			IsActorBase() const;
	virtual bool			IsMobileObject() const;
	virtual bool			IsActor() const;
	virtual uint32_t		Unk_41() const; // Used by TESPackage
	virtual void			Copy(TESForm* apCopy);
	virtual bool			Compare(TESForm* apForm);
	virtual bool			BelongsInGroup(FORM* apGroupFORM, bool abAllowParentGroups, bool abCurrentOnly) const;
	virtual void			CreateGroupData(FORM* apForm, FORM* apOutGroupFORM) const;
	virtual bool			IsParentForm() const;
	virtual bool			IsParentFormTree() const;
	virtual bool			IsFormTypeChild(uint8_t ucFormType) const;
	virtual bool			Activate(TESObjectREFR* apItemActivated, TESObjectREFR* apActionRef, bool abSound, TESBoundObject* apObjectToGet, int32_t aiCount);
	virtual void			SetFormID(uint32_t auiID, bool abUpdateFile);
	virtual const char*		GetObjectTypeName() const;
	virtual const char*		GetFormEditorID() const;
	virtual bool			SetFormEditorID(const char* edid);

	struct EditorData {
		BSString	strEditorID;
		uint32_t	uiVCMasterFormID;
		uint32_t	uiVCRevision;
	};

	enum Flags : uint32_t {
		IS_MASTER				= 1u << 0,
		IS_ALTERED				= 1u << 1,
		INITIALIZED				= 1u << 3,
		UNK_4					= 1u << 4,
		DELETED					= 1u << 5,
		KNOWN					= 1u << 6,
		IN_PLACEABLE_WATER		= 1u << 6,
		UNK_7					= 1u << 7,
		DROPPED					= 1u << 8,
		CASTS_SHADOWS			= 1u << 9,
		QUEST_ITEM				= 1u << 10,
		PERSISTENT				= 1u << 10,
		DISABLED				= 1u << 11,
		UNK_12					= 1u << 12,
		EMPTY					= 1u << 13,
		RESET_DESTRUCT			= 1u << 13,
		DONT_SAVE				= 1u << 14,
		TEMPORARY				= 1u << 14,
		VISIBLE_DISTANT			= 1u << 15,
		HAVOK_DEATH				= 1u << 16,
		NEED_TO_CHANGE_PROCESS	= 1u << 17,
		COMPRESSED				= 1u << 18,
		TARGETED				= 1u << 18, // TESObjectREFR
		SPECIFIC_TEXTURES		= 1u << 19,
		HAS_TEMP_3D				= 1u << 19, // TESObjectREFR
		CENTER_ON_CREATION		= 1u << 20,
		STILL_LOADING			= 1u << 21,
		BEING_DROPPED			= 1u << 22,
		UNK_23					= 1u << 23,
		UNK_24					= 1u << 24,
		OBSTACLE				= 1u << 25,
		IS_VATS_TARGETTABLE		= 1u << 26,
		DISABLE_FADE			= 1u << 27, // TESObjectREFR
		CHANGED_INVENTORY		= 1u << 27,
		UNK_28					= 1u << 28,
		UNK_29					= 1u << 29,
		TALKING_ACTIVATOR		= 1u << 30,
		CONTINUOUS_BROADCAST	= 1u << 30,
		UNK_31					= 1u << 31,

		TAKEN = DELETED | IS_ALTERED,
	};

	enum {
		kModified_Flags = 0x00000001
	};

	uint8_t					ucFormType;
#if JIP_CHANGES
	Bitfield8				ucJIPFormFlags5;
	Bitfield16				usJIPFormFlags6;
#endif
	Bitfield32				uiFormFlags;
	uint32_t				uiFormID;
	BSSimpleList<TESFile*>	kFiles;

	inline uint32_t		GetFormID() const { return uiFormID; }
	inline uint32_t		GetFormType() const { return ucFormType; }
	
	static FORM_ENUM_STRING* GetFormEnumString(uint8_t aucFormID);
};

ASSERT_SIZE(TESForm, 0x18);
