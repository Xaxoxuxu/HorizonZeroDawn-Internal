#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "player.hpp"
#include "consoleHelper.hpp"
#include "settings.hpp"

DWORD CleanupAndExit(const HMODULE hModule, const int exitCode, const ConsoleHelper ch)
{
	ch.DestroyConsole();
	FreeLibraryAndExitThread(hModule, exitCode);
}

uintptr_t FindDmaAddy(uintptr_t ptr, const std::vector<unsigned int>& offsets)
{
	uintptr_t address = ptr;
	for (const unsigned int offset : offsets)
	{
		const auto temp = reinterpret_cast<uintptr_t*>(address);
		if (!*temp)
		{
#if _DEBUG
			std::cerr << "[FindDmaAddy] nullptr at pointer '" << temp << "' at offset '" << offset << "'\n";
#endif
			return NULL;
		}

		address = *temp;
		address += offset;
	}
	return address;
}

void ShowMenu()
{
	const std::string hpToggle{ Settings::bFreezeHealth ? "ON" : "OFF" };
	const std::string crazyArrowsToggle{ Settings::bCrazyArrows ? "ON" : "OFF" };

	std::cout << "*******************************\n";
	std::cout << " NUM1 - Immortal. [" << hpToggle << "]\n";
	std::cout << " NUM2 - Crazy arrows. [" << crazyArrowsToggle << "]\n";
	std::cout << " DEL - Exit.\n";
	std::cout << "*******************************\n\n";
}

void Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void Patch(BYTE* dst, const BYTE* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void MainLoop(const ConsoleHelper& console)
{
	const auto moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

	while (true)
	{
		console.ClearScreen();
		ShowMenu();
		if (GetAsyncKeyState(VK_DELETE) & 1)
		{
			break;
		}

		const auto localPlayerPtr = reinterpret_cast<uintptr_t*>(FindDmaAddy(moduleBase + 0x0711C0B8, { 0x48 , 0x20 ,0x0, 0x1D0 }));
		if (!localPlayerPtr)
		{
			std::cerr << "LocalPlayer is NULL, are you in game?\nRetrying...\n";
			continue;
		}

		const auto me = reinterpret_cast<Player*>(*localPlayerPtr);
		if (!me)
		{
			std::cerr << "Player pointer is NULL, are you in game?\nRetrying...\n";
			continue;
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			Settings::bFreezeHealth = !Settings::bFreezeHealth;
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			Settings::bCrazyArrows = !Settings::bCrazyArrows;
		}

#if _DEBUG
		std::cout << "================== DEBUG ==================" << "\n";
		std::cout << "Module base: 0x" << std::hex << std::uppercase << moduleBase << "\n";
		std::cout << "LocalPlayerPtr: 0x" << std::hex << std::uppercase << localPlayerPtr << "\n";
		std::cout << "LocalPlayer: 0x" << std::hex << std::uppercase << *localPlayerPtr << "\n";
		std::cout << "mePtr: 0x" << std::hex << std::uppercase << me << "\n";
		std::cout << "me: 0x" << std::hex << std::uppercase << *reinterpret_cast<uintptr_t*>(me) << "\n";
		std::cout << "Crazy Arrows bytes start: 0x" << reinterpret_cast<uintptr_t*>(moduleBase + 0xFE1D20) << "\n";
		std::cout << "===========================================" << "\n";
#endif

		std::cout << "Health: " << me->health << "\n";
		std::cout << "Max Health: " << me->maxHealth << "\n";

		if (Settings::bFreezeHealth)
		{
			me->health = me->maxHealth;
		}

		if (Settings::bCrazyArrows)
		{
			Nop(reinterpret_cast<BYTE*>(moduleBase) + 0xFE1D20, 3);
		}
		else
		{
			Patch(reinterpret_cast<BYTE*>(moduleBase) + 0xFE1D20, (BYTE*)"\x89\x41\x58", 3);
		}

		Sleep(20);
	}
}

DWORD WINAPI MainThread(const HMODULE hModule)
{
	ConsoleHelper ch;

	if (!ch.InitConsole())
	{
		// TODO: print this error in some way, probably inside the InitConsole() function as well, or msgbox?
		return CleanupAndExit(hModule, EXIT_FAILURE, ch);
	}

	ch.ShowConsoleCursor(false);

	MainLoop(ch);

	return CleanupAndExit(hModule, EXIT_SUCCESS, ch);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		// Unnecessary thread calls disabled 
		DisableThreadLibraryCalls(hModule);

		const HANDLE threadHandle{ CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hModule, 0, nullptr) };
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
#if _DEBUG
		MessageBox(NULL, L"Detached", L"Information", MB_OK);
#endif
	}
	return TRUE;
}
