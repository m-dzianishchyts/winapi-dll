#include "MemoryFindReplace.h"

#include <vector>

namespace MemoryFindReplace {

	BOOL FindReplaceWithBuffer(PVOID regionBegin, CONST std::vector<CHAR> &regionCopy,
	                           CONST CHAR *target, SIZE_T targetSize, CONST CHAR *replacement, SIZE_T replacementSize);
	VOID ReplaceTargetAccessSpoof(HANDLE process, PVOID regionTarget, CONST CHAR *replacement, SIZE_T replacementSize);

	BOOL FindReplace(CONST VOID *target, SIZE_T targetSize, CONST VOID *replacement, SIZE_T replacementSize) {
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		DWORD pageSize = systemInfo.dwPageSize;
		auto minAddress = static_cast<PCHAR>(systemInfo.lpMinimumApplicationAddress);
		auto maxAddress = static_cast<PCHAR>(systemInfo.lpMaximumApplicationAddress);
		HANDLE currentProcess = GetCurrentProcess();

		std::vector<MEMORY_BASIC_INFORMATION> regionsBasicInformation;
		PCHAR address = minAddress;
		while (address <= maxAddress) {
			MEMORY_BASIC_INFORMATION memoryBasicInformation = {};
			SIZE_T bytesReturned = VirtualQuery(address, &memoryBasicInformation, sizeof(memoryBasicInformation));
			if (!bytesReturned) {
				address += pageSize;
				continue;
			}
			if (memoryBasicInformation.State == MEM_COMMIT && !(memoryBasicInformation.Protect & PAGE_GUARD)) {
				regionsBasicInformation.push_back(memoryBasicInformation);
			}
			address += memoryBasicInformation.RegionSize;
		}

		for (auto &regionBasicInformation : regionsBasicInformation) {
			std::vector<CHAR> buffer(regionBasicInformation.RegionSize);
			SIZE_T bytesRead = 0;
			BOOL readSuccessfully = ReadProcessMemory(currentProcess, regionBasicInformation.BaseAddress,
			                                          &buffer[0], buffer.size(), &bytesRead);
			if (!readSuccessfully) {
				DWORD error = GetLastError();
				if (error != ERROR_PARTIAL_COPY) {
					return FALSE;
				}
			}
			buffer.resize(bytesRead);
			FindReplaceWithBuffer(regionBasicInformation.BaseAddress, buffer,
			                      static_cast<CONST CHAR*>(target), targetSize,
			                      static_cast<CONST CHAR*>(replacement), replacementSize);
		}

		return TRUE;
	}

	BOOL FindReplaceWithBuffer(PVOID regionBegin, CONST std::vector<CHAR> &regionCopy,
	                           CONST CHAR *target, SIZE_T targetSize, CONST CHAR *replacement, SIZE_T replacementSize) {
		HANDLE currentProcess = GetCurrentProcess();
		SIZE_T matchLength = 0;
		SIZE_T regionPosition = 0;
		while (regionPosition < regionCopy.size()) {
			if (regionCopy[regionPosition] == target[matchLength]) {
				matchLength++;
				if (matchLength == targetSize) {
					PVOID regionTarget = static_cast<PCHAR>(regionBegin) + regionPosition - matchLength + 1;
					if (regionTarget != target) {
						ReplaceTargetAccessSpoof(currentProcess, regionTarget, replacement, replacementSize);
					}
					matchLength = 0;
				}
			} else {
				matchLength = 0;
			}
			regionPosition++;
		}
		return TRUE;
	}

	VOID ReplaceTargetAccessSpoof(HANDLE process, PVOID regionTarget, CONST CHAR *replacement, SIZE_T replacementSize) {
		DWORD oldProtect;
		VirtualProtectEx(process, regionTarget, replacementSize, PAGE_EXECUTE_READWRITE, &oldProtect);
		WriteProcessMemory(process, regionTarget, replacement, replacementSize, NULL);
		VirtualProtectEx(process, regionTarget, replacementSize, oldProtect, &oldProtect);
	}
}
