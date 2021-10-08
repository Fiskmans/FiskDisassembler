#define NOMINMAX
#include <windows.h>

#include "ConsoleHelp.h"
#include "Printers.h"
#include "CodeExplorer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


std::vector<unsigned char>
readAll(
	const char*									aFile)
{
	std::ifstream binary(aFile, std::ios::binary | std::ios::ate);

	if (!binary)
	{
		std::cout << "Failed to open file!" << std::endl;
		return {};
	}

	size_t size = binary.tellg();
	binary.seekg(std::ios_base::beg);

	std::vector<unsigned char> data;
	data.resize(size);
	binary.read(reinterpret_cast<char*>(data.data()), size);
	return data;
}


std::vector<unsigned char>
AssembleImage(
	size_t										aSize,
	size_t										aBaseAddress,
	const std::vector<unsigned char>&			aFileData,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections)
{
	std::vector<unsigned char> image;
	image.resize(aSize);
	for (unsigned char& c : image)
		c = '\0';

	for (const IMAGE_SECTION_HEADER& section : aSections)
	{
		if (section.PointerToRawData == 0)
			continue;

		if (section.PointerToRawData + section.SizeOfRawData > aFileData.size())
		{
			PRINTF_RED("Section goes past EOF\n");
			return {};
		}

		memcpy(
			image.data() + section.VirtualAddress,
			aFileData.data() + section.PointerToRawData,
			std::min(section.SizeOfRawData, section.Misc.VirtualSize));
	}

	return image;
}


int
main(int argc, const char** argv)
{
	if (argc < 2)
	{
		PRINTF_RED("No files specified on command line\n");
		system("pause");
		return EXIT_FAILURE;
	}
	for (int i = 1; i < argc; i++)
	{
		printf("==================================Disassembeling %s==================================",argv[i]);
		std::vector<unsigned char> fileContent = readAll(argv[i]);

		size_t readOffset = 0;

		PrintAround(fileContent, readOffset);
		IMAGE_DOS_HEADER dosHeader;
		memcpy(&dosHeader, fileContent.data() + readOffset, sizeof(dosHeader));
		readOffset += sizeof(dosHeader);
		PrintDosHeader(dosHeader);

		readOffset = dosHeader.e_lfanew;

		PrintAround(fileContent, readOffset);
		SIGNATURE signature;
		memcpy(&signature, fileContent.data() + readOffset, sizeof(signature));
		readOffset += sizeof(signature);
		PrintSignature(signature);

		PrintAround(fileContent, readOffset);
		IMAGE_FILE_HEADER coffHeader;
		memcpy(&coffHeader, fileContent.data() + readOffset, sizeof(coffHeader));
		readOffset += sizeof(coffHeader);
		PrintCoffHeader(coffHeader);

		IMAGE_OPTIONAL_HEADER32 optional32;
		PrintAround(fileContent, readOffset);
		memcpy(&optional32, fileContent.data() + readOffset, sizeof(optional32));
		PrintOptional(optional32, true);

		std::vector<IMAGE_DATA_DIRECTORY> dataDirectories;
		size_t	imageSize		   = 0;
		size_t	imageBase		   = 0;
		size_t	entryPointAddress   = 0;

		if (optional32.Magic == 0x20b)
		{
			IMAGE_OPTIONAL_HEADER64 optional64;
			PrintAround(fileContent, readOffset);
			memcpy(&optional64, fileContent.data() + readOffset, sizeof(optional64));
			PrintOptional(optional64, false);
			imageSize			= optional64.SizeOfImage;
			imageBase			= optional64.ImageBase;
			entryPointAddress	= optional64.AddressOfEntryPoint;
			dataDirectories.resize(optional64.NumberOfRvaAndSizes);
			memcpy(dataDirectories.data(), optional64.DataDirectory, sizeof(IMAGE_DATA_DIRECTORY) * optional64.NumberOfRvaAndSizes);
		}
		else
		{
			PrintOptional(optional32, false);
			imageSize			= optional32.SizeOfImage;
			imageBase			= optional32.ImageBase;
			entryPointAddress	= optional32.AddressOfEntryPoint;
			dataDirectories.resize(optional32.NumberOfRvaAndSizes);
			memcpy(dataDirectories.data(), optional32.DataDirectory, sizeof(IMAGE_DATA_DIRECTORY) * optional32.NumberOfRvaAndSizes);
		}
		PrintDirectories(dataDirectories);

		readOffset += coffHeader.SizeOfOptionalHeader;

		std::vector<IMAGE_SECTION_HEADER> sectionHeaders;
		sectionHeaders.resize(coffHeader.NumberOfSections);
		memcpy(sectionHeaders.data(), fileContent.data() + readOffset, sizeof(IMAGE_SECTION_HEADER) * coffHeader.NumberOfSections);

		PrintSections(sectionHeaders);

		std::vector<unsigned char> image = AssembleImage(imageSize, imageBase, fileContent, sectionHeaders);
	

		std::vector<Symbol> symbols = DumpImportedSymbols(image, dataDirectories);
		//DumpDebugTable(image, dataDirectories, sectionHeaders);

		ExploreCode(image, imageBase, entryPointAddress, sectionHeaders, symbols);

		system("pause");
	}

	return EXIT_SUCCESS;
}
