#include <windows.h>
#include <tlhelp32.h>

#include <iostream>

bool GetProcID(const char* procname, DWORD* id)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot == INVALID_HANDLE_VALUE) return false;
	PROCESSENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);
	Process32First(snapshot, &entry);
	do
	{
		if (strcmp(entry.szExeFile, procname) == 0) 
		{
			*id = entry.th32ProcessID;
			CloseHandle(snapshot);
			return true;
		}
	} while (Process32Next(snapshot, &entry));
	CloseHandle(snapshot);
	return false;
}

int main(int argc, char** argv)
{
	// Specify the names of the library and the program in order to load the desired library into the desired program.
	const char* procname = "Project1.exe";
	const char* dllname = "cheat.dll";
	// Get full path. (LoadLibraryA needs).
	const size_t bufsize = MAX_PATH + 1;
	char fullname[bufsize];
	if(!GetFullPathNameA(dllname, bufsize, fullname, 0)) return -1;
	DWORD id;
	
	// Get process id.
	for (; !GetProcID(procname, &id); Sleep(100));

	// Get handle to process by id. Win api func need handle to program.
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, id);
	if (process == NULL) return -2;

	// Allocate memory for path in program remotely.
	void* alloc = VirtualAllocEx(process, NULL, bufsize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (alloc == NULL)
	{
		CloseHandle(process);
		return -3;
	}

	// Write path to allocated memory.
	if(WriteProcessMemory(process, alloc, fullname, bufsize, NULL) == NULL)
	{
		CloseHandle(process);
		VirtualFreeEx(process, alloc, bufsize, MEM_RELEASE);
		return -4;
	}

	// Start LoadLibraryA func remotely.
	HANDLE thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, alloc, NULL, NULL);
	if (thread == NULL)
	{
		CloseHandle(process);
		VirtualFreeEx(process, alloc, bufsize, MEM_RELEASE);
		return -5;
	}

	// If LoadLibraryA ended work. (Library successfully loaded) close everything and exit.
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);

	// Free allocated memory for path.
	VirtualFreeEx(process, alloc, bufsize, MEM_RELEASE);

	// Close handle.
	CloseHandle(process);
	return 0;
}