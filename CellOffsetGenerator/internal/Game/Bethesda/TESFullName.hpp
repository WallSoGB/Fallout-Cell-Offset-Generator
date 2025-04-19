#pragma once

#include "BaseFormComponent.hpp"
#include "BSString.hpp"

class TESFile;
class TESForm;

class TESFullName : public BaseFormComponent {
public:
	TESFullName();
	~TESFullName();

	BSString	strFullName;
};

ASSERT_SIZE(TESFullName, 0xC);