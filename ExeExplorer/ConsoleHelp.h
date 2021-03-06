#pragma once
#define NOMINMAX
#include <windows.h>

void	ResetColor();
void	SetColorRed();
void	SetColorGreen();
void	SetColorPurple();
void	SetColorBlue();
void	SetColorLightBlue();

void	SetColumn(size_t aColumn);

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
#define PRINTF_PURPLE(...)   \
	{                        \
		SetColorPurple();    \
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