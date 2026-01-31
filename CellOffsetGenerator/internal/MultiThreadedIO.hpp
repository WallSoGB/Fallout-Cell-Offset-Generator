#pragma once

namespace MultiThreadedIO {
	void InitHooks() {
		PatchMemoryNop(0xAA306A, 5);
		WriteRelJump(0xAA85C0, 0xECB65A);
		WriteRelJump(0xAA8610, 0xECB3A8);
		WriteRelJump(0xAA8660, 0xECB086);
	}
}