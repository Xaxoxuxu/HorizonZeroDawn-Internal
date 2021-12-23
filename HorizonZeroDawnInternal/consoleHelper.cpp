#include "pch.h"
#include "consoleHelper.hpp"

bool ConsoleHelper::InitConsole()
{
	if (AllocConsole() == 0)
	{
		return false;
	}

	errno_t err = 0;
	err = freopen_s(&m_fInStream, "CONIN$", "r", stdin);
	if (err != 0)
	{
		return false;
	}

	err = freopen_s(&m_fOutStream, "CONOUT$", "w", stdout);
	if (err != 0)
	{
		return false;
	}

	err = freopen_s(&m_fErrStream, "CONOUT$", "w", stderr);
	if (err != 0)
	{
		return false;
	}

	return true;
}

void ConsoleHelper::DestroyConsole() const
{
	fclose(m_fInStream);
	fclose(m_fOutStream);
	fclose(m_fErrStream);
	FreeConsole();
}

ConsoleHelper::ConsoleHelper() : m_fInStream(nullptr), m_fOutStream(nullptr), m_fErrStream(nullptr)
{
}