#include <complex>
#include <iostream>

#include <iostream>
#include <Windows.h>

const std::string PATH_TO_DLL = R"(C:\Users\maxiemar\source\repos\winapi-dll\x64\Debug\MemoryFindReplace.dll)";

HANDLE RequestProcessId();
FARPROC LoadFunction(const std::string &moduleName, const std::string &functionName);
LPVOID InjectInProcessMemory(HANDLE process, CONST VOID* data, SIZE_T dataLength);
HANDLE RunInNewThread(HANDLE process, FARPROC function, LPVOID parameter);
void ReportLastError(const std::string &message);

constexpr auto OPEN_PROCESS_FAIL = 1;
constexpr auto GET_PROC_ADDRESS_FAIL = 2;
constexpr auto GET_MODULE_HANDLE_FAIL = 3;
constexpr auto VIRTUAL_ALLOC_FAIL = 4;
constexpr auto WRITE_PROCESS_MEMORY_FAIL = 5;
constexpr auto CREATE_REMOTE_THREAD_FAIL = 6;

int main() {
	HANDLE otherProcess = RequestProcessId();
	FARPROC loadLibraryFunction = LoadFunction("kernel32", "LoadLibraryA");
	LPVOID pathToDllBuffer = InjectInProcessMemory(otherProcess, PATH_TO_DLL.data(), PATH_TO_DLL.length());
	HANDLE remoteThread = RunInNewThread(otherProcess, loadLibraryFunction, pathToDllBuffer);

	std::cout << "DLL injection starting..." << std::endl;
	WaitForSingleObject(remoteThread, INFINITE);
	std::cout << "Done." << std::endl;

	CloseHandle(otherProcess);
	system("pause");

	return 0;
}

HANDLE RequestProcessId() {
	DWORD processId;
	std::cout << "Enter PID of the process to inject DLL into: ";
	std::cin >> processId;
	HANDLE otherProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (otherProcess == NULL) {
		ReportLastError("Process was not found.");
		throw OPEN_PROCESS_FAIL;
	}
	return otherProcess;
}

FARPROC LoadFunction(const std::string &moduleName, const std::string &functionName) {
	HMODULE moduleHandle = GetModuleHandleA(moduleName.c_str());
	if (moduleHandle == NULL) {
		ReportLastError("kernel32 loading failed.");
		throw GET_MODULE_HANDLE_FAIL;
	}
	FARPROC loadLibraryFunction = GetProcAddress(moduleHandle, functionName.c_str());
	if (loadLibraryFunction == NULL) {
		ReportLastError("LoadLibraryA function address resolving failed.");
		throw GET_PROC_ADDRESS_FAIL;
	}
	return loadLibraryFunction;

}

LPVOID InjectInProcessMemory(HANDLE process, CONST VOID *data, SIZE_T dataLength) {
	LPVOID buffer = VirtualAllocEx(process, NULL, PATH_TO_DLL.length(),
	                               MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (buffer == NULL) {
		ReportLastError("Buffer allocation failed.");
		throw VIRTUAL_ALLOC_FAIL;
	}

	SIZE_T bytesWritten;
	BOOL writeProcessResult = WriteProcessMemory(process, buffer, data, dataLength, &bytesWritten);
	if (!writeProcessResult) {
		ReportLastError("Writing data into memory failed.");
		throw WRITE_PROCESS_MEMORY_FAIL;
	}

	if (bytesWritten < PATH_TO_DLL.length()) {
		ReportLastError("Data was written incompletely.");
	}
	return buffer;
}

HANDLE RunInNewThread(HANDLE process, FARPROC function, LPVOID parameter) {
	HANDLE remoteThread = CreateRemoteThread(process, NULL, 0,
	                                         reinterpret_cast<LPTHREAD_START_ROUTINE>(function),
	                                         parameter, NULL, NULL);
	if (remoteThread == NULL) {
		ReportLastError("The remote thread was not created.");
		throw CREATE_REMOTE_THREAD_FAIL;
	}
	return remoteThread;
}

void ReportLastError(const std::string &message) {
	DWORD lastError = GetLastError();
	std::cout << message << " Error: " << lastError << std::endl;
}
