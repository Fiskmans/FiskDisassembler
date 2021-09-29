#pragma once
#define NOMINMAX
#include <windows.h>

void	ResetColor();
void	SetColorRed();
void	SetColorGreen();
void	SetColorBlue();
void	SetColorLightBlue();

#define PRINTF_RED(...)      \
	{                        \
		SetColorRed();       \
		printf(__VA_ARGS__); \
		ResetColor();        \
	}
#define PRINTF_GREEN(...)    \
	{                        \
		SetColorGreen();     \
		printf(__VA_ARGS__); \
		ResetColor();        \
	}
#define PRINTF_BLUE(...)     \
	{                        \
		SetColorBlue();      \
		printf(__VA_ARGS__); \
		ResetColor();        \
	}
#define PRINTF_LIGHTBLUE(...) \
	{                         \
		SetColorLightBlue();  \
		printf(__VA_ARGS__);  \
		ResetColor();         \
	}