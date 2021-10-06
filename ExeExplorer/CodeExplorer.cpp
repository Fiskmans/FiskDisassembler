#include "CodeExplorer.h"
#include "Printers.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

#include "CodeCommon.h"

#include "ModRMExtended.h"
#include "PrimaryOpCodeTable.h"
#include "SecondaryOpCodeTable.h"
size_t
ModRMExtension(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase,
	size_t										aImageBase)
{
	std::map<unsigned char, std::map<unsigned char, Instruction>> ModRMExtesionTable;

	ModRMExtesionTable[0xc0][0x0]	= ROL_RM;
	ModRMExtesionTable[0xc0][0x1]	= ROR_RM;
	ModRMExtesionTable[0xc0][0x4]	= SAL_RM;
	ModRMExtesionTable[0xc0][0x5]	= SHR_RM;

	ModRMExtesionTable[0xc1][0x0]	= ROL_RM;
	ModRMExtesionTable[0xc1][0x1]	= ROR_RM;
	ModRMExtesionTable[0xc1][0x4]	= SAL_RM;
	ModRMExtesionTable[0xc1][0x5]	= SHR_RM;

	ModRMExtesionTable[0xd0][0x0]	= ROL_RM;
	ModRMExtesionTable[0xd0][0x1]	= ROR_RM;
	ModRMExtesionTable[0xd0][0x4]	= SAL_RM;
	ModRMExtesionTable[0xd0][0x5]	= SHR_RM;

	ModRMExtesionTable[0xd1][0x0]	= ROL_RM;
	ModRMExtesionTable[0xd1][0x1]	= ROR_RM;
	ModRMExtesionTable[0xd1][0x4]	= SAL_RM;
	ModRMExtesionTable[0xd1][0x5]	= SHR_RM;

	ModRMExtesionTable[0xd2][0x0]	= ROL_RM;
	ModRMExtesionTable[0xd2][0x1]	= ROR_RM;
	ModRMExtesionTable[0xd2][0x4]	= SAL_RM;
	ModRMExtesionTable[0xd2][0x5]	= SHR_RM;

	ModRMExtesionTable[0xd3][0x0]	= ROL_RM;
	ModRMExtesionTable[0xd3][0x1]	= ROR_RM;
	ModRMExtesionTable[0xd3][0x4]	= SAL_RM;
	ModRMExtesionTable[0xd3][0x5]	= SHR_RM;

	ModRMExtesionTable[0x80][0x0]	= ADD_RM;
	ModRMExtesionTable[0x80][0x1]	= OR_RM;
	ModRMExtesionTable[0x80][0x4]	= AND_RM;
	ModRMExtesionTable[0x80][0x5]	= SUB_RM;
	ModRMExtesionTable[0x80][0x6]	= XOR_RM;
	ModRMExtesionTable[0x80][0x7]	= CMP_RM;

	ModRMExtesionTable[0x81][0x0]	= ADD_RM;
	ModRMExtesionTable[0x81][0x1]	= OR_RM;
	ModRMExtesionTable[0x81][0x4]	= AND_RM;
	ModRMExtesionTable[0x81][0x5]	= SUB_RM;
	ModRMExtesionTable[0x81][0x6]	= XOR_RM;
	ModRMExtesionTable[0x81][0x7]	= CMP_RM;

	ModRMExtesionTable[0x83][0x0]	= ADD_RM;
	ModRMExtesionTable[0x83][0x1]	= OR_RM;
	ModRMExtesionTable[0x83][0x4]	= AND_RM;
	ModRMExtesionTable[0x83][0x5]	= SUB_RM;
	ModRMExtesionTable[0x83][0x6]	= XOR_RM;
	ModRMExtesionTable[0x83][0x7]	= CMP_RM;

	ModRMExtesionTable[0xc7][0x0]	= MOV_RM;
	ModRMExtesionTable[0xc6][0x0]	= MOV_RM;

	ModRMExtesionTable[0xf6][0x0]	= TEST_RM;
	ModRMExtesionTable[0xf6][0x2]	= NOT_RM;

	ModRMExtesionTable[0xf7][0x0]	= TEST_RM;
	ModRMExtesionTable[0xf7][0x2]	= NOT_RM;

	ModRMExtesionTable[0xfe][0x0]	= INC_RM;
	ModRMExtesionTable[0xfe][0x1]	= DEC_RM;

	ModRMExtesionTable[0xff][0x0]	= INC_RM;
	ModRMExtesionTable[0xff][0x1]	= DEC_RM;
	ModRMExtesionTable[0xff][0x2]	= std::bind(CALL_RM, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	ModRMExtesionTable[0xff][0x3]	= std::bind(CALL_RM, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	ModRMExtesionTable[0xff][0x4]	= JMP_RM;
	ModRMExtesionTable[0xff][0x5]	= JMP_RM;

	if (ModRMExtesionTable.count(aImage[aExecutionPointer]) != 0)
	{
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

		if (ModRMExtesionTable[aImage[aExecutionPointer]].count(byte.reg) != 0)
		{
			return ModRMExtesionTable[aImage[aExecutionPointer]][byte.reg](aImage, aExecutionPointer + 1, aSections, aREX, aIs16Bit, aInstructionBase);
		}
		else
		{
			PRINTF_RED("Unkown ModRM extension instruction: %01x\n", byte.reg);
			PrintAround(aImage, aExecutionPointer);
			return -1;
		}
	}
	else
	{
		PRINTF_RED("Unkown ModRM extension group\n");
		PrintAround(aImage, aExecutionPointer);
		return -1;
	}
}

size_t
SecondaryModRMExtension(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	std::map<unsigned char, std::map<unsigned char, Instruction>> ModRMExtesionTable;

	ModRMExtesionTable[0x01][0x02] = XGETSETBV_sec_rm;

	if (ModRMExtesionTable.count(aImage[aExecutionPointer]) != 0)
	{
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

		if (ModRMExtesionTable[aImage[aExecutionPointer]].count(byte.reg) != 0)
		{
			return ModRMExtesionTable[aImage[aExecutionPointer]][byte.reg](aImage, aExecutionPointer + 1, aSections, aREX, aIs16Bit, aInstructionBase);
		}
		else
		{
			PRINTF_RED("Unkown ModRM extension instruction: %01x\n", byte.reg);
			PrintAround(aImage, aExecutionPointer);
			return -1;
		}
	}
	else
	{
		PRINTF_RED("Unkown ModRM extension group\n");
		PrintAround(aImage, aExecutionPointer);
		return -1;
	}
}

size_t
LegacyPrefix(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase,
	const std::map<unsigned char, Instruction>& aOpcodeTable)
{
	unsigned char nextInstruction = aImage[aExecutionPointer + 1];
	if (aExecutionPointer > aInstructionBase + 3)
	{
		PRINTF_RED("Too many legacy prefixes");
		PrintAround(aImage, aInstructionBase, 15);
		return -1;
	}

	switch (aImage[aExecutionPointer])
	{
	case 0xf3:
		printf("REP ");
		break;
	case 0xf2:
		printf("REPNZ ");
		break;
	}

	std::map<unsigned char, Instruction>::const_iterator it = aOpcodeTable.find(nextInstruction);

	if (it != aOpcodeTable.cend())
	{
		return (it->second)(aImage, aExecutionPointer + 1, aSections, aREX, aIs16Bit, aInstructionBase);
	}
	else
	{
		PRINTF_RED("Unkown instruction: %02x\n", nextInstruction);
		PrintAround(aImage, aExecutionPointer + 1, aSections);
		return -1;
	}
}

size_t
WordSizePrefix(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase,
	const std::map<unsigned char, Instruction>& aOpcodeTable)
{
	unsigned char nextInstruction = aImage[aExecutionPointer + 1];

	std::map<unsigned char, Instruction>::const_iterator it = aOpcodeTable.find(nextInstruction);

	if (it != aOpcodeTable.cend())
	{
		return (it->second)(aImage, aExecutionPointer + 1, aSections, aREX, true, aInstructionBase);
	}
	else
	{
		PRINTF_RED("Unkown instruction: %02x\n", nextInstruction);
		PrintAround(aImage, aExecutionPointer + 1, aSections);
		return -1;
	}
}

size_t
REXPrefix(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase,
	const std::map<unsigned char, Instruction>& aOpcodeTable)
{
	unsigned char nextInstruction = aImage[aExecutionPointer + 1];

	if (nextInstruction >= 0x40 && nextInstruction <= 0x4F)
	{
		PRINTF_RED("Double REX prefix\n");
		PrintAround(aImage, aExecutionPointer + 1);
		return -1;
	}

	std::map<unsigned char, Instruction>::const_iterator it = aOpcodeTable.find(nextInstruction);

	if (it != aOpcodeTable.cend())
	{
		REXState rex;
		rex.w	= !!(aImage[aExecutionPointer] & 0b00001000);
		rex.r	= !!(aImage[aExecutionPointer] & 0b00000100);
		rex.x	= !!(aImage[aExecutionPointer] & 0b00000010);
		rex.b	= !!(aImage[aExecutionPointer] & 0b00000001);

		return (it->second)(aImage, aExecutionPointer + 1, aSections, rex, aIs16Bit, aInstructionBase);
	}
	else
	{
		PRINTF_RED("Unkown instruction: %02x\n", nextInstruction);
		PrintAround(aImage, aExecutionPointer + 1, aSections);
		return -1;
	}
}

size_t
SecondaryOPCodeTable(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	std::map<unsigned char, Instruction> secondaryOpcodeTable;

	secondaryOpcodeTable[0x01] = SecondaryModRMExtension;

	for (unsigned char	i   = 0x19; i <= 0x9f; i++)
		secondaryOpcodeTable[i] = NOP_sec;

	secondaryOpcodeTable[0xb0]	= CMPXCHG;
	secondaryOpcodeTable[0xb1]	= CMPXCHG;

	secondaryOpcodeTable[0xb6]	= MOVZX;
	secondaryOpcodeTable[0xb7]	= MOVZX;

	for (unsigned char	i   = 0x80; i <= 0x8f; i++)
		secondaryOpcodeTable[i] = JCC_sec;

	secondaryOpcodeTable[0xa2]		= CPUID_sec;

	unsigned char nextInstruction	= aImage[aExecutionPointer + 1];

	if (secondaryOpcodeTable.count(nextInstruction) != 0)
	{
		return secondaryOpcodeTable[nextInstruction](aImage, aExecutionPointer + 1, aSections, aREX, aIs16Bit, aInstructionBase);
	}
	else
	{
		PRINTF_RED("Unkown secondary opcode: %02x\n", nextInstruction);
		PrintAround(aImage, aExecutionPointer + 1, aSections);
		return -1;
	}
}

void
ExploreCode(
	const std::vector<unsigned char>&			aImage,
	size_t										aImageBase,
	size_t										aExecutionStart,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	const std::vector<Symbol>&					aSymbols)
{
	globals::globalImageBase = aImageBase;

	std::vector<std::pair<Symbol, bool>> symbols;
	for (const Symbol&	symbol : aSymbols)
	{
		symbols.push_back(std::make_pair(symbol, false));
	}

	std::map<unsigned char, Instruction> legacyOpCodeTable;

	for (unsigned char	i   = 0x00; i <= 0x05; i++)
		legacyOpCodeTable[i] = ADD;

	for (unsigned char	i   = 0x08; i <= 0x0D; i++)
		legacyOpCodeTable[i] = OR;

	legacyOpCodeTable[0x0F] = SecondaryOPCodeTable;

	for (unsigned char	i   = 0x20; i <= 0x25; i++)
		legacyOpCodeTable[i] = AND;

	legacyOpCodeTable[0x26] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));

	for (unsigned char	i   = 0x28; i <= 0x2D; i++)
		legacyOpCodeTable[i] = SUB;

	legacyOpCodeTable[0x2E] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));

	for (unsigned char	i   = 0x30; i <= 0x35; i++)
		legacyOpCodeTable[i] = XOR;

	legacyOpCodeTable[0x36] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));

	for (unsigned char	i   = 0x38; i <= 0x3D; i++)
		legacyOpCodeTable[i] = CMP;

	legacyOpCodeTable[0x3E] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));

	for (unsigned char	i   = 0x40; i <= 0x4F; i++)
		legacyOpCodeTable[i] = std::bind(REXPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));

	for (unsigned char	i   = 0x50; i <= 0x57; i++)
		legacyOpCodeTable[i] = PUSH;

	for (unsigned char	i   = 0x58; i <= 0x5F; i++)
		legacyOpCodeTable[i] = POP;

	legacyOpCodeTable[0x63] = MOVSXD;
	legacyOpCodeTable[0x64] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0x65] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0x66] = std::bind(WordSizePrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));

	legacyOpCodeTable[0x69] = IMUL;
	legacyOpCodeTable[0x6B] = IMUL;

	for (unsigned char	i   = 0x70; i <= 0x7F; i++)
		legacyOpCodeTable[i] = JCC;

	legacyOpCodeTable[0x80] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0x81] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0x82] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0x83] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);

	legacyOpCodeTable[0x84] = TEST;
	legacyOpCodeTable[0x85] = TEST;
	legacyOpCodeTable[0x86] = XCHG;
	legacyOpCodeTable[0x87] = XCHG;

	for (unsigned char	i   = 0x88; i <= 0x8C; i++)
		legacyOpCodeTable[i] = MOV;

	legacyOpCodeTable[0x8D] = LEA;

	legacyOpCodeTable[0x90] = CBW;

	legacyOpCodeTable[0xA4] = MOVS;
	legacyOpCodeTable[0xA5] = MOVS;

	legacyOpCodeTable[0xAA] = STOS;
	legacyOpCodeTable[0xAB] = STOS;

	for (unsigned char	i   = 0xB0; i <= 0xBF; i++)
		legacyOpCodeTable[i] = MOV;

	legacyOpCodeTable[0xC0] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0xC1] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);

	legacyOpCodeTable[0xC2] = RET;
	legacyOpCodeTable[0xC3] = RET;

	legacyOpCodeTable[0xC6] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0xC7] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0xCC] = INT3;
	legacyOpCodeTable[0xCD] = INT_Instruction;

	legacyOpCodeTable[0xD0] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0xD1] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0xD2] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0xD3] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);

	legacyOpCodeTable[0xF2] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0xF3] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::ref(legacyOpCodeTable));

	legacyOpCodeTable[0xF6] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);
	legacyOpCodeTable[0xF7] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);

	legacyOpCodeTable[0xE8] = CALL;
	legacyOpCodeTable[0xE9] = JMP;
	legacyOpCodeTable[0xEB] = JMP;

	legacyOpCodeTable[0xF0] = CLC;
	legacyOpCodeTable[0xFF] = std::bind(ModRMExtension, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, aImageBase);

	REXState rex;
	rex.w	= false;
	rex.r	= false;
	rex.x	= false;
	rex.b	= false;

	globals::globalFunctions.push_back({"Main", aExecutionStart});

	do
	{
		size_t executionPointer = -1;
		for (BranchType& function : globals::globalFunctions)
		{
			if (function.myExpanded)
				continue;

			printf("%08zx [", function.myAddress);
			PrintFunction(function.myAddress);
			printf("]");
			function.myExpanded = true;
			executionPointer	= function.myAddress;
			break;
		}

		if (executionPointer == -1)
			break;

		while (executionPointer != -1)
		{
			for (size_t i = 0; i < globals::globalBranches.size(); i++)
			{
				if (globals::globalBranches[i].myExpanded)
					continue;

				if (executionPointer == globals::globalBranches[i].myAddress)
				{
					printf("\n[");
					PrintLocation(executionPointer);
					printf("]");
					globals::globalBranches[i].myExpanded = true;
					break;
				}
			}

			bool isSymbol = false;
			for (std::pair<Symbol, bool>&		symbol : symbols)
			{
				if (symbol.second)
					continue;

				if (symbol.first.myAddress == executionPointer)
				{
					isSymbol		= true;
					symbol.second	= true;
					printf("\nImported %s symbol: [%s] ", symbol.first.myType.c_str(), symbol.first.mySource.c_str());
					PRINTF_PURPLE("%s\n\n", aImage.data() + symbol.first.myName);
					break;
				}
			}

			if (isSymbol)
				break;

			if (executionPointer >= aImage.size())
			{
				PRINTF_RED("\nRead Access Violation\n\n");
				break;
			}
			bool found = false;
			for (const IMAGE_SECTION_HEADER&	section : aSections)
			{
				if (executionPointer > section.VirtualAddress && executionPointer < section.VirtualAddress + section.Misc.VirtualSize)
				{
					if ((section.Characteristics & (IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE)) == (IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE))
					{
						found = true;
						break;
					}
				}
			}
			if (!found)
			{
				PRINTF_RED("\nRead Access Violation\n\n");
				break;
			}

			printf("\n  0x%08zx ", executionPointer);
			if (legacyOpCodeTable.count(aImage[executionPointer]))
			{
				executionPointer = legacyOpCodeTable[aImage[executionPointer]](aImage, executionPointer, aSections, rex, false, executionPointer);
			}
			else
			{
				PRINTF_RED("Unkown instruction: %02x\n", aImage[executionPointer]);
				PrintAround(aImage, executionPointer, aSections);
				executionPointer = -1;
			}

			if (executionPointer == -1)
			{
				for (BranchType& b : globals::globalBranches)
				{
					if (!b.myExpanded)
					{
						executionPointer = b.myAddress;
						break;
					}
				}
			}
		}
		printf("\n\n");
	} while (true);

	printf("\Symbols:");
	for (auto&			symbol : symbols)
	{
		if (symbol.second)
		{
			PRINTF_GREEN("\n\tSEEN    ");
		}
		else
		{
			PRINTF_RED("\n\tUNSEEN  ");
		}

		PRINTF_PURPLE("%s", aImage.data() + symbol.first.myName);
	}
	printf("\n");
}