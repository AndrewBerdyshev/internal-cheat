#include "memory.h"

void __stdcall Unload(HINSTANCE instance)
{
	// Free resources and unload library. Program still work.
	Sleep(100);
	FreeLibraryAndExitThread(instance, 0);
}

void __stdcall Main(HINSTANCE instance)
{
	// Show message box to notify user about success start.
	MessageBoxA(0, "I'm inside!", "Ur nightmare", MB_OK);
	// Get pointer to a variable. 12 - pattern length after that our variable exists.
	char* addr = (FindPattern("\x50\x10\x3F\x00\x88\x13\x00\x00", "xxx?xx??")+12);

	// Endless loop. Press end to stop loop and uload library.
	while (!GetAsyncKeyState(VK_END))
	{
		// Write number to a variable id delete key pressed.
		if (GetAsyncKeyState(VK_DELETE))
		{
			// Cast char* to int* as we wanna write integer.
			*reinterpret_cast<int*>(addr) = 10;
		}
	}

	// Start unload func.
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Unload, instance, 0, 0);
}

// Start point of dll.
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	// Discard everything except the start.
	if (fdwReason != DLL_PROCESS_ATTACH) return TRUE;

	// Create thread to return result to our injector and continue working.
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, hinstDLL, 0, 0);

	return TRUE;
}