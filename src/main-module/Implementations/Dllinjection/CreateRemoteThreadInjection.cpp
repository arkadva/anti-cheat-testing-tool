// author: houjingyi233
// https://raw.githubusercontent.com/houjingyi233/dll-injection-by-CreateRemoteThread/master/Source.cpp

#include <Windows.h>
#include <stdio.h> 
#include "../../Utils/utilities.h"
#include "../../Utils/logger.h"
#include "CreateRemoteThreadInjection.h"

HANDLE RtlCreateUserThread(HANDLE hProcess, LPVOID lpBaseAddress, LPVOID lpSpace) {
	// the prototype of RtlCreateUserThread from undocumented.ntinternals.com
	typedef DWORD(WINAPI* functypeRtlCreateUserThread)(
		HANDLE      ProcessHandle,
		PSECURITY_DESCRIPTOR  SecurityDescriptor,
		BOOL      CreateSuspended,
		ULONG     StackZeroBits,
		PULONG     StackReserved,
		PULONG     StackCommit,
		LPVOID     StartAddress,
		LPVOID     StartParameter,
		HANDLE      ThreadHandle,
		LPVOID     ClientID
	);

	// get handle for ntdll which contains RtlCreateUserThread
	HANDLE hRemoteThread = NULL;
	HMODULE hNtDllModule = GetModuleHandleW(L"ntdll.dll");
	if (hNtDllModule == NULL) {
		return NULL;
	}

	functypeRtlCreateUserThread funcRtlCreateUserThread = (functypeRtlCreateUserThread) GetProcAddress(hNtDllModule, "RtlCreateUserThread");
	if (!funcRtlCreateUserThread) {
		return NULL;
	}

	funcRtlCreateUserThread(hProcess, NULL, 0, 0, 0, 0, lpBaseAddress, lpSpace, &hRemoteThread, NULL);
	DWORD lastError = GetLastError();
	return hRemoteThread;
}

void LogThread(HANDLE hThread, const char* method) {
	Logger& logger = Logger::getInstance();

	DWORD threadId = GetThreadId(hThread);
	DWORD processId = GetProcessIdOfThread(hThread);
	LOG_INFO("Injected thread id: %u for pid: %u using %s.", threadId, processId, method);
}

HANDLE NtCreateThreadEx(HANDLE hProcess, LPVOID lpBaseAddress, LPVOID lpSpace) {
	// the prototype of NtCreateThreadEx from undocumented.ntinternals.com
	typedef DWORD(WINAPI* functypeNtCreateThreadEx)(
		PHANDLE                 ThreadHandle,
		ACCESS_MASK             DesiredAccess,
		LPVOID                  ObjectAttributes,
		HANDLE                  ProcessHandle,
		LPTHREAD_START_ROUTINE  lpStartAddress,
		LPVOID                  lpParameter,
		BOOL                    CreateSuspended,
		DWORD                   dwStackSize,
		DWORD                   Unknown1,
		DWORD                   Unknown2,
		LPVOID                  Unknown3
	);

	HANDLE                      hRemoteThread = NULL;
	HMODULE                     hNtDllModule = NULL;
	functypeNtCreateThreadEx    funcNtCreateThreadEx = NULL;

	// get handle for ntdll which contains NtCreateThreadEx
	hNtDllModule = GetModuleHandleW(L"ntdll.dll");
	if (hNtDllModule == NULL) {
		return NULL;
	}

	funcNtCreateThreadEx = (functypeNtCreateThreadEx) GetProcAddress(hNtDllModule, "NtCreateThreadEx");
	if (!funcNtCreateThreadEx) {
		return NULL;
	}

	funcNtCreateThreadEx(&hRemoteThread, GENERIC_ALL, NULL, hProcess, (LPTHREAD_START_ROUTINE) lpBaseAddress, lpSpace, FALSE, NULL, NULL, NULL, NULL);
	return hRemoteThread;
}

int injectIntoPID(DWORD pid, BYTE method, const wchar_t* dll) {
	Logger& logger = Logger::getInstance();
#ifdef DEBUG
	LOG_CALL("pid = %lu, method = %d, dll = %s", pid, method, dll);
#endif

	// gets the process handle for the target process
	UINT handleAccess = PROCESS_ALL_ACCESS;
	HANDLE hProcess = OpenProcess(handleAccess, FALSE, pid);

	if (hProcess == NULL) {
		LOG_ERROR("Unable to open handle.");
		return FALSE;
	}

	// retrieves kernel32.dll module handle for getting loadlibrary base address
	HMODULE hModule = GetModuleHandleW(L"kernel32.dll.");

	if (hModule == NULL) {
		LOG_ERROR("Unable to get handle to kernel32.dll.");
		return FALSE;
	}

	// gets address for LoadLibraryW in kernel32.dll
	LPVOID lpBaseAddress = (LPVOID) GetProcAddress(hModule, "LoadLibraryW");

	if (lpBaseAddress == NULL) {
		LOG_ERROR("Unable to locate LoadLibraryW.");
		return FALSE;
	}

	// allocates space inside for inject.dll to our target process
	LPVOID lpSpace = (LPVOID) VirtualAllocEx(hProcess, NULL, (wcslen(dll) + 1) * sizeof(wchar_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (lpSpace == NULL) {
		LOG_ERROR("Could not allocate memory.");
		return -1;
	}

	// write inject.dll to memory of process
	int n = WriteProcessMemory(hProcess, lpSpace, dll, (wcslen(dll) + 1) * sizeof(wchar_t), NULL);
	if (n == 0) {
		LOG_ERROR("Could not write to process' address space.");
		return -1;
	}

	HANDLE hThread;

	if (method & kNtCreateThreadEx) {
		hThread = NtCreateThreadEx(hProcess, lpBaseAddress, lpSpace);

		if (hThread) {
			LogThread(hThread, "NtCreateThreadEx");
		}
		else {
			LOG_ERROR("Creating thread using NtCreateThreadEx failed.");
		}
	}

	if (method & kRtlCreateUserThread) {
		hThread = RtlCreateUserThread(hProcess, lpBaseAddress, lpSpace);

		if (hThread) {
			LogThread(hThread, "RtlCreateUserThread");
		}
		else {
			LOG_ERROR("Creating thread using RtlCreateUserThread failed.");
		}
	}

	if (method & kCreateRemoteThread) {
		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) lpBaseAddress, lpSpace, NULL, NULL);

		if (hThread) {
			LogThread(hThread, "CreateRemoteThread");
		}
		else {
			LOG_ERROR("Creating thread using CreateRemoteThread failed.");
		}
	}

	CloseHandle(hProcess);
}
