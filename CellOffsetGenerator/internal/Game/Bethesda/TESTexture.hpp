#pragma once

#include "BaseFormComponent.hpp"
#include "BSString.hpp"

class TESTexture : public BaseFormComponent {
public:
	TESTexture();
	~TESTexture();

	virtual uint32_t		GetMaxAllowedSize() const;
	virtual const char* GetAsNormalFile(BSString& arStr) const;
	virtual const char* GetDefaultPath() const;

	BSString strTextureName;

	const char* GetTextureName() const;
	uint32_t GetTextureNameLength() const;
};

ASSERT_SIZE(TESTexture, 0xC);