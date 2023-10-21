#include <windows.h>
#include <tlhelp32.h>

// Func returns module struct by it's name.
bool GetModule(DWORD pID, const char* modulename, MODULEENTRY32* module)
{
	// Module allows you to get information about the status of applications running on the local computer.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	MODULEENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);

	// A loop through all the modules to find the right one by checking the names.
	Module32First(snapshot, &entry);
	do
	{
		if (strcmp(modulename, entry.szModule) == 0) 
		{
			*module = entry;
			CloseHandle(snapshot);
			return true;
		}
	} while (Module32Next(snapshot, &entry));

	module = nullptr;
	// End working with model.
	CloseHandle(snapshot);
	return false;
}

// Func returns pointer to byte where pattern starts in module.
char* FindPatternInModule(const char* module, const char* pattern, const char* mask)
{
	// Get module info.
	MODULEENTRY32 mInfo;
	if (!GetModule(GetCurrentProcessId(), module, &mInfo)) return NULL;
	char* base = mInfo.modBaseAddr;
	int size = mInfo.modBaseSize;
	// Get patterns length.
	int patternLength = strlen(mask);

	// Loop through memory to find pattern.
	for (int i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (int j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}
		if (found) return base + i;
	}
	return NULL;
}

// Func returns pointer to byte where pattern starts in all memory.
char* FindPattern(const char* pattern, const char* mask)
{
	// Struct to save regions' info.
	MEMORY_BASIC_INFORMATION mbi{};
	// Pattern length
	int patternLength = strlen(mask);

	// Loop through memory(x86).
	for (char* curr = 0; (DWORD)curr < 0xFFFFFFFF; curr += mbi.RegionSize)
	{
		// Check accessibility to region, get it's info.
		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS || mbi.Protect == 0x104) continue;

		// Loop through region.
		for (int i = 0; i < mbi.RegionSize; i++)
		{
			bool found = true;
			for (int j = 0; j < patternLength; j++)
			{
				found &= mask[j] == '?' || pattern[j] == *(char*)(curr + i + j);
				if (!found)
					break;
			}
			if(found)return curr + i;
		}
	}
	return nullptr;
}