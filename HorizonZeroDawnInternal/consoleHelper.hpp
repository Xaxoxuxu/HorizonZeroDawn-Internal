#pragma once
#include <Windows.h>
#include <iostream>

class ConsoleHelper
{
public:
	ConsoleHelper();
	~ConsoleHelper() = default;

	/// <summary>
	/// <para>Initialize console.</para>
	/// <para>TODO: Function should be restructured since 2 types of error responses can be encountered - GetLastError and errno_t, 
	/// and we are checking, but not printing them in any way (log?)</para>
	/// </summary>
	/// <returns>true on success, false on failure</returns>
	bool InitConsole();

	void DestroyConsole() const;
private:
	FILE* m_fInStream, * m_fOutStream, * m_fErrStream;
};