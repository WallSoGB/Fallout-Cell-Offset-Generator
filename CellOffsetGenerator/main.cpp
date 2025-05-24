#include "BSMemory.hpp"
#include "CellOffsetGenerator.hpp"
#include "InteriorOffsets.hpp"
#include "MultiThreadedIO.hpp"
#include "nvse/PluginAPI.h"

IDebugLog gLog("logs\\CellOffsetGenerator.log");

BS_ALLOCATORS

static void NVSEMessageHandler(NVSEMessagingInterface::Message* apMessage) {
	switch (apMessage->type) {
	case NVSEMessagingInterface::kMessage_DeferredInit: 
		{
			OffsetGenerator kGenerator;
			kGenerator.RenderUI();
			gLog.~IDebugLog();
		}
		break;
	}
}

EXTERN_DLL_EXPORT bool NVSEPlugin_Preload() {
	// Undo Obsidian's serialized stdio
	MultiThreadedIO::InitHooks();

	return true;
}


EXTERN_DLL_EXPORT bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info) {
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "Cell Offset Generator";
	info->version = 104;

	return !nvse->isEditor;
}

EXTERN_DLL_EXPORT bool NVSEPlugin_Load(NVSEInterface* nvse) {
	if (!nvse->isEditor) {
		OffsetGenerator::InitHooks();
		InteriorOffsets::InitHooks();
		auto pMessageInterface = static_cast<NVSEMessagingInterface*>(nvse->QueryInterface(kInterface_Messaging));
		pMessageInterface->RegisterListener(nvse->GetPluginHandle(), "NVSE", NVSEMessageHandler);
	}

	return true;
}

BOOL WINAPI DllMain(
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
)
{
	return TRUE;
}