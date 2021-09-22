#pragma once
#define NOMINMAX
#include <windows.h>

void	ResetColor();
void	SetColorRed();
void	SetColorGreen();

#define PRINTF_GREEN(...)    \
	{                        \
		SetColorGreen();     \
		printf(__VA_ARGS__); \
		ResetColor();        \
	}
#define PRINTF_RED(...)      \
	{                        \
		SetColorRed();       \
		printf(__VA_ARGS__); \
		ResetColor();        \
	}