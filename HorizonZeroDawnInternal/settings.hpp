#pragma once

class Settings
{
public:
	Settings() = delete;

	// use atomic_bools if thread-safety is needed

	inline static bool bFreezeHealth{ false };
	inline static bool bCrazyArrows{ false };
};
