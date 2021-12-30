#pragma once
#include <atomic>

class Settings
{
public:
	Settings() = delete;

	inline static std::atomic_bool bFreezeHealth{ false };
	inline static std::atomic_bool bCrazyArrows{ false };
};
