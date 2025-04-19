#pragma once

namespace MultiThreadedIO {
	namespace ToReplace {
		constexpr static uint32_t fseekAddr = 0xAA171B;
		constexpr static uint32_t freadAddr[] = { 0xAA1583, 0xAA17AF, 0xAA17CF };
		constexpr static uint32_t fwriteAddr[] = { 0xAA15CD, 0xAA1881 };
	}

	namespace FunctionsAddr {
		constexpr static uint32_t fseek = 0xECB65A;
		constexpr static uint32_t fread = 0xECB3A8;
		constexpr static uint32_t fwrite = 0xECB086;
	}

	void InitHooks() {
		PatchMemoryNop(0xAA306A, 5);

		ReplaceCall(ToReplace::fseekAddr, FunctionsAddr::fseek);

		for (uint32_t addr : ToReplace::freadAddr)
			ReplaceCall(addr, FunctionsAddr::fread);

		for (uint32_t addr : ToReplace::fwriteAddr)
			ReplaceCall(addr, FunctionsAddr::fwrite);
	}
}