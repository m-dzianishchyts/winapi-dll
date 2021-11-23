#pragma once

#include "framework.h"

#ifdef DYNAMIC_FIND_REPLACE_LIB_EXPORTS
#define DYNAMIC_FIND_REPLACE_LIB_API __declspec(dllexport)
#else
#define DYNAMIC_FIND_REPLACE_LIB_API __declspec(dllimport)
#endif

namespace MemoryFindReplace {

	extern "C" DYNAMIC_FIND_REPLACE_LIB_API BOOL FindReplace(CONST VOID *target, SIZE_T targetSize,
	                                                         CONST VOID *replacement, SIZE_T replacementSize);
}
