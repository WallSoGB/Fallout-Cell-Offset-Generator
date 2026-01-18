#pragma once

namespace MultiThreadedIO {
	namespace ToReplace {
		constexpr static uint32_t fseekAddr		= 0xAA85C0;
		constexpr static uint32_t freadAddr		= 0xAA8610;
		constexpr static uint32_t fwriteAddr	= 0xAA8660;
	}

	namespace FunctionsAddr {
		constexpr static uint32_t fseek		= 0xECB65A;
		constexpr static uint32_t fread		= 0xECB3A8;
		constexpr static uint32_t fwrite	= 0xECB086;
	}

	void InitHooks() {
		PatchMemoryNop(0xAA306A, 5);

		WriteRelJump(ToReplace::fseekAddr, FunctionsAddr::fseek);

		WriteRelJump(ToReplace::freadAddr, FunctionsAddr::fread);

		WriteRelJump(ToReplace::fwriteAddr, FunctionsAddr::fwrite);
	}
}