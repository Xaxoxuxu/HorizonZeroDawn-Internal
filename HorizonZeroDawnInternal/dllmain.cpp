#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include "consoleHelper.hpp"

DWORD cleanupAndExit(const HMODULE hModule, const int exitCode, const ConsoleHelper ch)
{
	ch.DestroyConsole();
	FreeLibraryAndExitThread(hModule, exitCode);
}

void mainLoop()
{
	while (true)
	{
		Sleep(10);
	}
}

DWORD WINAPI MainThread(const HMODULE hModule)
{
	ConsoleHelper ch;

	if (!ch.InitConsole())
	{
		// TODO: print this error in some way, probably inside the InitConsole() function as well, or msgbox?
		return cleanupAndExit(hModule, EXIT_FAILURE, ch);
	}

	std::cout << "Successfully attached!" << std::endl;
	Sleep(2000);
	//mainLoop();

	return cleanupAndExit(hModule, EXIT_SUCCESS, ch);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		// Unnecessary thread calls disabled 
		DisableThreadLibraryCalls(hModule);

		const HANDLE threadHandle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hModule, 0, nullptr);
		if (threadHandle != NULL)
		{
			CloseHandle(threadHandle);
		}
		else
		{
			// TODO: check with GetLastError
			// GetLastError (how do I print the error? logging system?) and exit????
		}
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		MessageBox(
			NULL,
			L"Detached",
			L"Information",
			MB_OK
		);
	}
	return TRUE;
}
