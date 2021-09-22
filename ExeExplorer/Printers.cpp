#include "Printers.h"

void
PrintAround(
	const std::vector<unsigned char>&			aData,
	size_t										aPointer,
	size_t										aSize)
{
	printf("\n");

	printf("0x%016x:", static_cast<unsigned int>(aPointer));
	for (size_t i	= aPointer > aSize ? aPointer - aSize : 0; i < aPointer + aSize && i < aData.size(); i++)
	{
		printf(" %02x", static_cast<int>(aData[i]));
	}
	printf("\n                   ");
	for (size_t i	= aPointer > aSize ? aPointer - aSize : 0; i < aPointer + aSize && i < aData.size(); i++)
	{
		char buffer[2] = ".";
		if (isprint(aData[i]))
			buffer[0] = aData[i];

		printf("  %s", buffer);
	}
	printf("\n                   ");
	for (size_t i	= aPointer > aSize ? aPointer - aSize : 0; i < aPointer + aSize && i < aData.size(); i++)
	{
		printf(" %s", i == aPointer ? "^^" : "  ");
	}
	printf("\n");
}

void
PrintAround(
	const std::vector<unsigned char>&			aData,
	size_t										aPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	size_t										aSize)
{
	printf("\n");

	printf("0x%016x:", static_cast<unsigned int>(aPointer));
	for (size_t i	= aPointer > aSize ? aPointer - aSize : 0; i < aPointer + aSize && i < aData.size(); i++)
	{
		printf(" %02x", static_cast<int>(aData[i]));
	}

	bool found = false;
	for (const IMAGE_SECTION_HEADER& section : aSections)
	{
		if (aPointer >= section.VirtualAddress && aPointer < section.VirtualAddress + section.Misc.VirtualSize)
		{
			found = true;
			char name[9];
			memcpy(name, section.Name, 8);
			name[8] = '\0';

			printf(" [%s] ", name);
			const char* separator = "";
			if ((section.Characteristics & IMAGE_SCN_TYPE_NO_PAD) != 0)
			{
				printf("%sTYPE_NO_PAD ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_CNT_CODE) != 0)
			{
				PRINTF_GREEN("%sCNT_CODE ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) != 0)
			{
				PRINTF_GREEN("%sCNT_INITIALIZED_DATA ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) != 0)
			{
				printf("%sCNT_UNINITIALIZED_DATA ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_LNK_OTHER) != 0)
			{
				printf("%sLNK_OTHER ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_LNK_INFO) != 0)
			{
				printf("%sLNK_INFO ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_LNK_REMOVE) != 0)
			{
				printf("%sLNK_REMOVE ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_LNK_COMDAT) != 0)
			{
				printf("%sLNK_COMDAT ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_GPREL) != 0)
			{
				printf("%sGPREL ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_PURGEABLE) != 0)
			{
				printf("%sMEM_PURGEABLE ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_16BIT) != 0)
			{
				printf("%sMEM_16BIT ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_LOCKED) != 0)
			{
				printf("%sMEM_LOCKED ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_PRELOAD) != 0)
			{
				printf("%sMEM_PRELOAD ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_1BYTES) != 0)
			{
				printf("%sALIGN_1BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_2BYTES) != 0)
			{
				printf("%sALIGN_2BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_4BYTES) != 0)
			{
				printf("%sALIGN_4BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_8BYTES) != 0)
			{
				printf("%sALIGN_8BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_16BYTES) != 0)
			{
				printf("%sALIGN_16BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_32BYTES) != 0)
			{
				printf("%sALIGN_32BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_64BYTES) != 0)
			{
				printf("%sALIGN_64BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_128BYTES) != 0)
			{
				printf("%sALIGN_128BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_256BYTES) != 0)
			{
				printf("%sALIGN_256BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_512BYTES) != 0)
			{
				printf("%sALIGN_512BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_1024BYTES) != 0)
			{
				printf("%sALIGN_1024BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_2048BYTES) != 0)
			{
				printf("%sALIGN_2048BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_4096BYTES) != 0)
			{
				printf("%sALIGN_4096BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_ALIGN_8192BYTES) != 0)
			{
				printf("%sALIGN_8192BYTES ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL) != 0)
			{
				printf("%sLNK_NRELOC_OVFL ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_DISCARDABLE) != 0)
			{
				printf("%sMEM_DISCARDABLE ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_NOT_CACHED) != 0)
			{
				printf("%sMEM_NOT_CACHED ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_NOT_PAGED) != 0)
			{
				printf("%sMEM_NOT_PAGED ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_SHARED) != 0)
			{
				printf("%sMEM_SHARED ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0)
			{
				PRINTF_GREEN("%sMEM_EXECUTE ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_READ) != 0)
			{
				printf("%sMEM_READ ", separator);
				separator = "| ";
			}
			if ((section.Characteristics & IMAGE_SCN_MEM_WRITE) != 0)
			{
				printf("%sMEM_WRITE ", separator);
				separator = "| ";
			}
			break;
		}
	}
	if (!found)
	{
		printf(" [Outside of initialized memory]");
	}

	printf("\n                   ");
	for (size_t i	= aPointer > aSize ? aPointer - aSize : 0; i < aPointer + aSize && i < aData.size(); i++)
	{
		char buffer[2] = ".";
		if (isprint(aData[i]))
			buffer[0] = aData[i];

		printf("  %s", buffer);
	}
	printf("\n                   ");
	for (size_t i	= aPointer > aSize ? aPointer - aSize : 0; i < aPointer + aSize && i < aData.size(); i++)
	{
		printf(" %s", i == aPointer ? "^^" : "  ");
	}
	printf("\n");
}

void
PrintDosHeader(
	const IMAGE_DOS_HEADER&						aHeader)
{

	printf("Dos header:\n");
	printf(
		"\tMagic:          %04x ", aHeader.e_magic);
	if (aHeader.e_magic == IMAGE_DOS_SIGNATURE)
	{
		PRINTF_GREEN("MZ");
	}
	else
	{
		PRINTF_RED("%c%c", aHeader.e_magic & 0xFF, (aHeader.e_magic & 0xFF00) >> 8);
	}
	printf("\n");

	printf(
		"\tLast page size: %04x\n"
		"\tPage count:     %04x\n"
		"\tRelocations:    %04x\n"
		"\tHeaders allocs: %04x\n"
		"\tMin allocs:     %04x\n"
		"\tMax allocs:     %04x\n"
		"\tInitial SS:     %04x\n"
		"\tInitial SP:     %04x\n"
		"\tChecksum:       %04x\n"
		"\tInitial IP:     %04x\n"
		"\tInitial CS:     %04x\n"
		"\tReloc location: %04x\n"
		"\tOverlay number: %04x\n"
		"\tReserved 1.1:   %04x\n"
		"\tReserved 1.2:   %04x\n"
		"\tReserved 1.3:   %04x\n"
		"\tReserved 1.4:   %04x\n"
		"\tOEM id:         %04x\n"
		"\tOEM info:       %04x\n"
		"\tReserved 2.1:   %04x\n"
		"\tReserved 2.2:   %04x\n"
		"\tReserved 2.3:   %04x\n"
		"\tReserved 2.4:   %04x\n"
		"\tReserved 2.5:   %04x\n"
		"\tReserved 2.6:   %04x\n"
		"\tReserved 2.7:   %04x\n"
		"\tReserved 2.8:   %04x\n"
		"\tReserved 2.9:   %04x\n"
		"\tReserved 2.10:  %04x\n"
		"\tNew Header:     %08x\n",
		aHeader.e_cblp,
		aHeader.e_cp,
		aHeader.e_crlc,
		aHeader.e_cparhdr,
		aHeader.e_minalloc,
		aHeader.e_maxalloc,
		aHeader.e_ss,
		aHeader.e_sp,
		aHeader.e_csum,
		aHeader.e_ip,
		aHeader.e_cs,
		aHeader.e_lfarlc,
		aHeader.e_ovno,
		aHeader.e_res[0],
		aHeader.e_res[1],
		aHeader.e_res[2],
		aHeader.e_res[3],
		aHeader.e_oemid,
		aHeader.e_oeminfo,
		aHeader.e_res2[0],
		aHeader.e_res2[1],
		aHeader.e_res2[2],
		aHeader.e_res2[3],
		aHeader.e_res2[4],
		aHeader.e_res2[5],
		aHeader.e_res2[6],
		aHeader.e_res2[7],
		aHeader.e_res2[8],
		aHeader.e_res2[9],
		aHeader.e_lfanew);
}

void
PrintCoffHeader(
	const IMAGE_FILE_HEADER&					aHeader)
{
	printf(
		"\tMachine:              ");

	printf("%04x ", aHeader.Machine);

	switch (aHeader.Machine)
	{
	case IMAGE_FILE_MACHINE_AMD64:
		PRINTF_GREEN("Amd64\n");
		break;
	default:
		PRINTF_RED("Unkown machine\n");
		break;
	}

	printf(
		"\tNumberOfSections:     %04x\n"
		"\tTimeDateStamp:        %08x\n"
		"\tPointerToSymbolTable: %08x\n"
		"\tNumberOfSymbols:      %08x\n"
		"\tSizeOfOptionalHeader: %04x\n"
		"\tCharacteristics:      %04x ",
		aHeader.NumberOfSections,
		aHeader.TimeDateStamp,
		aHeader.PointerToSymbolTable,
		aHeader.NumberOfSymbols,
		aHeader.SizeOfOptionalHeader,
		aHeader.Characteristics);

	const char* seperator = "";
	if ((aHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) != 0)
	{
		printf("%sIMAGE_FILE_RELOCS_STRIPPED ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) != 0)
	{
		PRINTF_GREEN("%sIMAGE_FILE_EXECUTABLE_IMAGE ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_LINE_NUMS_STRIPPED) != 0)
	{
		printf("%sIMAGE_FILE_LINE_NUMS_STRIPPED ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_LOCAL_SYMS_STRIPPED) != 0)
	{
		printf("%sIMAGE_FILE_LOCAL_SYMS_STRIPPED ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE) != 0)
	{
		printf("%sIMAGE_FILE_LARGE_ADDRESS_AWARE ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_BYTES_REVERSED_LO) != 0)
	{
		printf("%sIMAGE_FILE_BYTES_REVERSED_LO ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_32BIT_MACHINE) != 0)
	{
		printf("%sIMAGE_FILE_32BIT_MACHINE ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED) != 0)
	{
		printf("%sIMAGE_FILE_DEBUG_STRIPPED ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP) != 0)
	{
		printf("%sIMAGE_FILE_REMOVABLE_RUN_FROM_SWAP ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_NET_RUN_FROM_SWAP) != 0)
	{
		printf("%sIMAGE_FILE_NET_RUN_FROM_SWAP ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_SYSTEM) != 0)
	{
		printf("%sIMAGE_FILE_SYSTEM ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_DLL) != 0)
	{
		printf("%sIMAGE_FILE_DLL ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_UP_SYSTEM_ONLY) != 0)
	{
		printf("%sIMAGE_FILE_UP_SYSTEM_ONLY ", seperator);
		seperator = "| ";
	}
	if ((aHeader.Characteristics & IMAGE_FILE_BYTES_REVERSED_HI) != 0)
	{
		printf("%sIMAGE_FILE_BYTES_REVERSED_HI ", seperator);
		seperator = "| ";
	}

	printf("\n\n");
}

void
PrintSignature(
	const SIGNATURE&							aSignature)
{

	printf(
		"signature:           %08x ", *reinterpret_cast<const unsigned int*>(aSignature.data));

	if (memcmp(aSignature.data, "PE\0\0", 4) == 0)
	{
		PRINTF_GREEN("PE\\0\\0");
	}
	else
	{
		PRINTF_RED("Unkown signature!");
	}

	printf("\n");
}

void
PrintOptional(
	const IMAGE_OPTIONAL_HEADER32&				aHeader,
	bool										aJustMagic)
{
	printf("Optional header32:\n");
	printf("\tMagic number: ");
	if (aHeader.Magic == 0x10b)
	{
		PRINTF_GREEN("%04x\n", aHeader.Magic);
	}
	else
	{
		PRINTF_RED("%04x\n", aHeader.Magic);
	}

	if (aJustMagic)
		return;
}

void
PrintOptional(
	const IMAGE_OPTIONAL_HEADER64&				aHeader,
	bool										aJustMagic)
{
	printf("Optional header64:\n");
	printf("\tMagic number: ");
	if (aHeader.Magic == 0x20b)
	{
		PRINTF_GREEN("%04x\n", aHeader.Magic);
	}
	else
	{
		PRINTF_RED("%04x\n", aHeader.Magic);
	}

	if (aJustMagic)
		return;

	printf(
		"\n\t==Standard fields==\n"
		"\tMajor linker: %02x\n"
		"\tMinor Linker: %02x\n"
		"\tCode size:    %08x\n"
		"\tInit data:    %08x\n"
		"\tUninit data:  %08x\n"
		"\tEntry point:  %08x\n"
		"\tBase of code: %08x\n",
		aHeader.MajorLinkerVersion,
		aHeader.MinorLinkerVersion,
		aHeader.SizeOfCode,
		aHeader.SizeOfInitializedData,
		aHeader.SizeOfUninitializedData,
		aHeader.AddressOfEntryPoint,
		aHeader.BaseOfCode);

	printf(
		"\n\t==Windows specific==\n"
		"\tImage base:     %016I64x\n"
		"\tSection Align:  %08x\n"
		"\tFile Align:     %08x\n"
		"\tMajor OS:       %04x\n"
		"\tMinor OS:       %04x\n"
		"\tMajor Image:    %04x\n"
		"\tMinor Image:    %04x\n"
		"\tMajor Subsys:   %04x\n"
		"\tMinor Subsys:   %04x\n"
		"\tWin32 ver (res):%08x\n"
		"\tImage size:     %08x\n"
		"\tHeader size:    %08x\n"
		"\tChecksum:       %08x\n"
		"\tSubsystem:      %04x ",
		aHeader.ImageBase,
		aHeader.SectionAlignment,
		aHeader.FileAlignment,
		aHeader.MajorOperatingSystemVersion,
		aHeader.MinorOperatingSystemVersion,
		aHeader.MajorImageVersion,
		aHeader.MinorImageVersion,
		aHeader.MajorSubsystemVersion,
		aHeader.MinorSubsystemVersion,
		aHeader.Win32VersionValue,
		aHeader.SizeOfImage,
		aHeader.SizeOfHeaders,
		aHeader.CheckSum,
		aHeader.Subsystem);

	switch (aHeader.Subsystem)
	{

	case IMAGE_SUBSYSTEM_NATIVE:
		PRINTF_GREEN("Native\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_GUI:
		PRINTF_GREEN("Gui\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_CUI:
		PRINTF_GREEN("Cui (Console)\n");
		break;
	case IMAGE_SUBSYSTEM_OS2_CUI:
		PRINTF_GREEN("OS2 Cui (console)\n");
		break;
	case IMAGE_SUBSYSTEM_POSIX_CUI:
		PRINTF_GREEN("Posix Cui (console)\n");
		break;
	case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:
		PRINTF_RED("Native win9x driver\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
		PRINTF_RED("CE gui\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_APPLICATION:
		PRINTF_RED("EFI app\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER:
		PRINTF_RED("EFI boot\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER:
		PRINTF_RED("EFI driver\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_ROM:
		PRINTF_RED("EFI rom\n");
		break;
	case IMAGE_SUBSYSTEM_XBOX:
		PRINTF_RED("XBOX\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION:
		PRINTF_RED("WIndows boot app\n");
		break;
	case IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG:
		PRINTF_RED("XBOX code catalog\n");
		break;

	case IMAGE_SUBSYSTEM_UNKNOWN:
	default:
		PRINTF_RED("Unkown subsystem\n");
		break;
	}

	printf(
		"\tDll char:       %04x ",
		aHeader.DllCharacteristics);

	const char* separator = "";
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA) != 0)
	{
		printf("%sHIGH_ENTROPY_VA ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE) != 0)
	{
		printf("%sDYNAMIC_BASE ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY) != 0)
	{
		printf("%sFORCE_INTEGRITY ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NX_COMPAT) != 0)
	{
		printf("%sNX_COMPAT ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_ISOLATION) != 0)
	{
		printf("%sNO_ISOLATION ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_SEH) != 0)
	{
		printf("%sNO_SEH ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_BIND) != 0)
	{
		printf("%sNO_BIND ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_APPCONTAINER) != 0)
	{
		printf("%sAPPCONTAINER ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_WDM_DRIVER) != 0)
	{
		printf("%sWDM_DRIVER ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_GUARD_CF) != 0)
	{
		printf("%sGUARD_CF ", separator);
		separator = "| ";
	}
	if ((aHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE) != 0)
	{
		printf("%sTERMINAL_SERVER_AWARE ", separator);
		separator = "| ";
	}
	printf("\n");

	printf(
		"\tStack reserve:  %016I64x\n"
		"\tStack commit:   %016I64x\n"
		"\tHeap reserve:   %016I64x\n"
		"\tHeap commit:    %016I64x\n"
		"\tLoader flags:   %08x\n"
		"\tNumber of rva:  %08x\n\n",
		aHeader.SizeOfStackReserve,
		aHeader.SizeOfStackCommit,
		aHeader.SizeOfHeapReserve,
		aHeader.SizeOfHeapCommit,
		aHeader.LoaderFlags,
		aHeader.NumberOfRvaAndSizes);
}

void
PrintDirectories(
	const std::vector<IMAGE_DATA_DIRECTORY>&	aDirectories)
{
	const char* names[] = {
		"Export table",
		"Import table",
		"Resource table",
		"Exception table",
		"Certificate table",
		"Base reloc table",
		"Debug",
		"Architecture",
		"Global ptr",
		"TLS table",
		"Load config table",
		"Bound import",
		"Import address table",
		"Delay import description",
		"CLR runtime header",
		"reserved"};

	printf("===DATA DIRECTORIES===\n");

	for (size_t i	= 0; i < aDirectories.size() && i < 16; i++)
	{
		printf(
			"\t[%s]:\n"
			"\t\tVirt address: %08x\n"
			"\t\tSize:         %08x\n\n",
			names[i],
			aDirectories[i].VirtualAddress,
			aDirectories[i].Size);
	}

	for (size_t i	= 16; i < aDirectories.size(); i++)
	{
		printf(
			"\t[Unkown data directory]:\n"
			"\t\tVirt address: %08x\n"
			"\t\tSize:         %08x\n\n",
			aDirectories[i].VirtualAddress,
			aDirectories[i].Size);
	}
}

void
PrintSections(
	const std::vector<IMAGE_SECTION_HEADER>&	aSections)
{
	printf("===SECTIONS===\n");
	for (const IMAGE_SECTION_HEADER& section : aSections)
	{
		char name[9];
		memcpy(name, section.Name, 8);
		name[8] = '\0';
		printf(
			"\t[%s]:\n"
			"\t\tVirtual size:      %08x\n"
			"\t\tVirtual address:   %08x\n"
			"\t\tRaw data size:     %08x\n"
			"\t\tRaw data location: %08x\n"
			"\t\tRelocs location:   %08x\n"
			"\t\tLines location:    %08x\n"
			"\t\tRelocs count:      %08x\n"
			"\t\tLines count:       %08x\n"
			"\t\tCharacteristics:   %016x ",
			name,
			section.Misc.VirtualSize,
			section.VirtualAddress,
			section.SizeOfRawData,
			section.PointerToRawData,
			section.PointerToRelocations,
			section.PointerToLinenumbers,
			section.NumberOfRelocations,
			section.NumberOfLinenumbers,
			section.Characteristics);

		const char* separator = "";
		if ((section.Characteristics & IMAGE_SCN_TYPE_NO_PAD) != 0)
		{
			printf("%sTYPE_NO_PAD ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_CNT_CODE) != 0)
		{
			PRINTF_GREEN("%sCNT_CODE ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) != 0)
		{
			PRINTF_GREEN("%sCNT_INITIALIZED_DATA ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) != 0)
		{
			printf("%sCNT_UNINITIALIZED_DATA ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_LNK_OTHER) != 0)
		{
			printf("%sLNK_OTHER ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_LNK_INFO) != 0)
		{
			printf("%sLNK_INFO ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_LNK_REMOVE) != 0)
		{
			printf("%sLNK_REMOVE ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_LNK_COMDAT) != 0)
		{
			printf("%sLNK_COMDAT ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_GPREL) != 0)
		{
			printf("%sGPREL ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_PURGEABLE) != 0)
		{
			printf("%sMEM_PURGEABLE ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_16BIT) != 0)
		{
			printf("%sMEM_16BIT ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_LOCKED) != 0)
		{
			printf("%sMEM_LOCKED ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_PRELOAD) != 0)
		{
			printf("%sMEM_PRELOAD ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_1BYTES) != 0)
		{
			printf("%sALIGN_1BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_2BYTES) != 0)
		{
			printf("%sALIGN_2BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_4BYTES) != 0)
		{
			printf("%sALIGN_4BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_8BYTES) != 0)
		{
			printf("%sALIGN_8BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_16BYTES) != 0)
		{
			printf("%sALIGN_16BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_32BYTES) != 0)
		{
			printf("%sALIGN_32BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_64BYTES) != 0)
		{
			printf("%sALIGN_64BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_128BYTES) != 0)
		{
			printf("%sALIGN_128BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_256BYTES) != 0)
		{
			printf("%sALIGN_256BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_512BYTES) != 0)
		{
			printf("%sALIGN_512BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_1024BYTES) != 0)
		{
			printf("%sALIGN_1024BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_2048BYTES) != 0)
		{
			printf("%sALIGN_2048BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_4096BYTES) != 0)
		{
			printf("%sALIGN_4096BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_ALIGN_8192BYTES) != 0)
		{
			printf("%sALIGN_8192BYTES ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_LNK_NRELOC_OVFL) != 0)
		{
			printf("%sLNK_NRELOC_OVFL ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_DISCARDABLE) != 0)
		{
			printf("%sMEM_DISCARDABLE ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_NOT_CACHED) != 0)
		{
			printf("%sMEM_NOT_CACHED ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_NOT_PAGED) != 0)
		{
			printf("%sMEM_NOT_PAGED ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_SHARED) != 0)
		{
			printf("%sMEM_SHARED ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0)
		{
			PRINTF_GREEN("%sMEM_EXECUTE ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_READ) != 0)
		{
			printf("%sMEM_READ ", separator);
			separator = "| ";
		}
		if ((section.Characteristics & IMAGE_SCN_MEM_WRITE) != 0)
		{
			printf("%sMEM_WRITE ", separator);
			separator = "| ";
		}

		printf("\n");
	}
}

void
DumpImportedSymbols(
	const std::vector<unsigned char>&			aData,
	const std::vector<IMAGE_DATA_DIRECTORY>		aDirectories)
{
	if (aDirectories.size() < 2)
	{
		PRINTF_RED("No symbols imported, NO IMPORTDIRECTORY");
		return;
	}

	if (aDirectories[1].Size == 0)
	{
		PRINTF_RED("No symbols imported, EMPTY IMPORTDIRECTORY");
		return;
	}
	printf("===IMPORTED SYMBOLS===\n");

	size_t readOffset = aDirectories[1].VirtualAddress;
	while (true)
	{
		IMAGE_IMPORT_DESCRIPTOR description;
		memcpy(&description, aData.data() + readOffset, sizeof(description));

		readOffset += sizeof(description);
		if (description.Name == 0)
			break;

		printf(
			"\t[%s]\n"
			"\t\tLookup table:                  %08x\n"
			"\t\tTimeDate:                      %08x\n"
			"\t\tForwarder chain:               %08x\n"
			"\t\tName:                          %08x\n"
			"\t\tImport address table location: %08x\n\n",
			aData.data() + description.Name,
			description.Characteristics,
			description.TimeDateStamp,
			description.ForwarderChain,
			description.Name,
			description.FirstThunk);

		size_t lookupReadOffset = description.Characteristics;
		while (true)
		{
			uint64_t importLookup;
			memcpy(&importLookup, aData.data() + lookupReadOffset, sizeof(importLookup));
			if (importLookup == 0)
				break;
			lookupReadOffset += sizeof(importLookup);

			if (IMAGE_SNAP_BY_ORDINAL64(importLookup))
			{
				printf("\t\tOrdinal: %04x", static_cast<uint16_t>(IMAGE_ORDINAL64(importLookup)));
			}
			else
			{
				uint16_t hint;
				memcpy(&hint, aData.data() + importLookup, sizeof(hint));
				printf("\t\t[%04x]:%s\n", hint, aData.data() + importLookup + sizeof(hint));
			}
		}
	}
}