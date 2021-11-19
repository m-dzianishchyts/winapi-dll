#include <string>

#include "framework.h"
#include "MemoryFindReplace.h"

const std::string TARGET =      "Hello World!";
const std::string REPLACEMENT = "I'm broken((";

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			MemoryFindReplace::FindReplace(TARGET.data(), TARGET.length(), REPLACEMENT.data(), REPLACEMENT.length());
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
		default: ;
	}
	return TRUE;
}
