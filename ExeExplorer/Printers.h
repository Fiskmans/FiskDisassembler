#pragma once
#include <vector>

#define NOMINMAX
#include <windows.h>

#include "ConsoleHelp.h"

void	PrintAround(
			const std::vector<unsigned char>&			aData,
			size_t										aPointer,
			size_t										aSize   = 8);

void	PrintAround(
			const std::vector<unsigned char>&			aData,
			size_t										aPointer,
			const std::vector<IMAGE_SECTION_HEADER>&	aSections,
			size_t										aSize   = 8);
void	PrintDosHeader(
			const IMAGE_DOS_HEADER&						aHeader);

void	PrintCoffHeader(
			const IMAGE_FILE_HEADER&					aHeader);

struct SIGNATURE
{
	unsigned char data[4];
};

void	PrintSignature(
			const SIGNATURE&							aSignature);

void	PrintOptional(
			const IMAGE_OPTIONAL_HEADER32&				aHeader,
			bool										aJustMagic);
void	PrintOptional(
			const IMAGE_OPTIONAL_HEADER64&				aHeader,
			bool										aJustMagic);
void	PrintDirectories(
			const std::vector<IMAGE_DATA_DIRECTORY>&	aDirectories);
void	PrintSections(
			const std::vector<IMAGE_SECTION_HEADER>&	aSections);

void	DumpImportedSymbols(
			const std::vector<unsigned char>&			aData,
			const std::vector<IMAGE_DATA_DIRECTORY>		aDirectories);

template <typename Type>
void
PrintSignedHex(
	Type												aValue);

template<>
void PrintSignedHex<int8_t>(
			int8_t						aValue);

template <>
void
PrintSignedHex<int16_t>(
	int16_t								aValue);

template <>
void
PrintSignedHex<int32_t>(
	int32_t								aValue);

template <>
void
PrintSignedHex<int64_t>(
	int64_t								aValue);