#include "TESForm.hpp"

#ifdef GAME
static FORM_ENUM_STRING* const pFormEnumStrings = (FORM_ENUM_STRING*)0x1187000;
#else
static FORM_ENUM_STRING* const pFormEnumStrings = (FORM_ENUM_STRING*)0xE94400;
#endif

FORM_ENUM_STRING* TESForm::GetFormEnumString(uint8_t aucFormID) {
	return &pFormEnumStrings[aucFormID];
}