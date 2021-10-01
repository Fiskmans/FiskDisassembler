#include "CodeExplorer.h"
#include "Printers.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

#include "CodeCommon.h"

#include "PrimaryOpCodeTable.h"
#include "ModRMExtended.h"
#include "SecondaryOpCodeTable.h"


size_t
JCC(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{

	switch (aImage[aExecutionPointer])
	{
	case 0x74: {
		printf("JZ  ");
		int8_t	offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));
		size_t	dest = aExecutionPointer + 1 + sizeof(offset) + offset;
		AddBranch(dest);

		PrintSignedHex(offset);
		SetColumn(globals::globalLocationColumn);
		printf("0x%08zx  ", dest);
		PrintLocation(dest);

		return aExecutionPointer + 2;
	}
	case 0x75: {
		printf("JZ  ");
		int8_t	offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));
		size_t	dest = aExecutionPointer + 1 + sizeof(offset) + offset;
		AddBranch(dest);

		PrintSignedHex(offset);
		SetColumn(globals::globalLocationColumn);
		printf("0x%08zx  ", dest);
		PrintLocation(dest);

		return aExecutionPointer + 2;
	}
	}

	printf("unkown jcc type");
	PrintAround(aImage, aExecutionPointer);

	return -1;
}


size_t
SUB_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);

	printf("SUB ");
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x80:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

	case 0x81:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
			
	case 0x83:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}
	return -1;
}

size_t
ADD_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("ADD ");
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x80:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x81:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x83:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("Unkown add_rm");
	return -1;
}

size_t
CALL_near_32_im(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("CALL ");
	int32_t offset;
	memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));

	size_t		nextInstruction = aExecutionPointer + 1 + sizeof(offset);
	size_t		target		   = nextInstruction + offset;
	std::string name			= GetOrGenerateFunctionName(target);

	AddFunction(target);

	PrintSignedHex(offset);
	SetColumn(globals::globalLocationColumn);
	printf("0x%08zx  ", target);
	PrintFunction(target);

	return nextInstruction;
}

size_t
CALL_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("CALL ");
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	size_t		out	   = -1;

	switch (byte.reg)
	{
	case 0x2: {

		size_t loc = 0;
		if (aREX.w)
			out = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1, false, &loc);
		else
			out = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1, false, &loc);

		if (loc != 0)
		{
			AddFunction(loc, "_memaddr");
			printf("\t");
			PrintFunction(loc);
		}
		else
		{
			PrintFunction(GenerateName("function_", "_regaddr"));
		}
	}
	break;
	default:
		break;
	}

	return out;
}

size_t
RET(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("RET");
	if (aImage[aExecutionPointer] == 0xC2)
	{
		uint16_t toPop;
		memcpy(&toPop, aImage.data() + aExecutionPointer + 1, sizeof(toPop));
		printf(" POP(%04x)", toPop);
	}
	return -1;
}

size_t
MOV_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("MOV ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xc6:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xc7:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM64, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	}

	printf("unkown mov_rm type");
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
MOVZX(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("MOVZX ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	switch (aImage[aExecutionPointer])
	{
	case 0xB6: {
		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	}
	case 0xB7: {
		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);
	}
	}

	printf("unkown movzx type %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer);
	return -1;
}

size_t
MOVS(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{

	switch (aImage[aExecutionPointer])
	{
	case 0xa4:
		printf("MOVS [rSI], [rDI]");
		break;
	case 0xa5:
		if (aREX.w)
			printf("MOVS [rSI], [rDI]");
		else
			printf("MOVS [rSI], [rDI]");

		break;
	}
	return aExecutionPointer + 1;
}

size_t
CMP(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("CMP ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
	switch (aImage[aExecutionPointer])
	{
	case 0x39: {
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	}
	break;
	}

	PRINTF_RED("unmapped cmp opcode %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
CMP_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("CMP ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x83: {
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}
	break;
	}

	PRINTF_RED("unmapped cmp_rm opcode %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
TEST(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("TEST ");

	switch (aImage[aExecutionPointer])
	{
	case 0x85: {
		ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer + 1], aREX);
		uint32_t	extra1	 = 0;
		uint32_t	extra2	 = 0;

		int32_t		offset	= 0;

		printf("%s", RegMem(byte, aREX, RegisterSize::REGISTER_64BIT, false, aImage, aExecutionPointer + 2, extra1, &offset).c_str());
		printf(", %s", RegMem(byte, aREX, RegisterSize::REGISTER_64BIT, true, aImage, aExecutionPointer + 2, extra2).c_str());

		if (offset != 0)
		{
			size_t loc = aExecutionPointer + 2 + extra1 + offset;
			printf("\t0x%08zx", loc);
			if (aREX.w)
			{
				uint64_t toCmp;
				memcpy(&toCmp, aImage.data() + loc, sizeof(toCmp));
				printf(": %016I64x", toCmp);
			}
		}

		return aExecutionPointer + 2 + extra1;
	}
	break;
	}

	printf("unmapped test opcode %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
NOT_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("NOT ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);
	return Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
}
size_t
XOR(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("XOR ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
	switch (aImage[aExecutionPointer])
	{
	case 0x30:
		return Operands(OperandType::REGMEM8, OperandType::REG8, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x31:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x32:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x33:
		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	default:
		break;
	}

	PRINTF_RED("unkown xor");
	PrintAround(aImage, aInstructionBase);
	return -1;
}

size_t
LEA(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("LEA ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	if (aREX.w)
		return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2, true);

	return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2, true);
}
size_t
CLC(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("CLC");
	return aExecutionPointer + 1;
}

size_t
CMPXCHG(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("CMPXCHG ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	switch (aImage[aExecutionPointer])
	{
	case 0xb0:
		return Operands(OperandType::REGMEM8, OperandType::REG8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0xb1:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);
		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	}

	PRINTF_RED("unkown cmpxchg");
	return -1;
}

size_t
ModRMExtension(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	std::map<unsigned char, std::map<unsigned char, Instruction>> ModRMExtesionTable;

	ModRMExtesionTable[0x81][0x0]	= ADD_RM;
	ModRMExtesionTable[0x81][0x5]	= SUB_RM;
	ModRMExtesionTable[0x81][0x7]	= CMP_RM;

	ModRMExtesionTable[0x83][0x0]	= ADD_RM;
	ModRMExtesionTable[0x83][0x5]	= SUB_RM;
	ModRMExtesionTable[0x83][0x7]	= CMP_RM;

	ModRMExtesionTable[0xc7][0x0]	= MOV_RM;
	ModRMExtesionTable[0xc6][0x0]	= MOV_RM;

	ModRMExtesionTable[0xf7][0x2]	= NOT_RM;

	ModRMExtesionTable[0xff][0x2]	= CALL_RM;
	ModRMExtesionTable[0xff][0x3]	= CALL_RM;
	ModRMExtesionTable[0xff][0x4]	= JMP_RM;
	ModRMExtesionTable[0xff][0x5]	= JMP_RM;

	if (ModRMExtesionTable.count(aImage[aExecutionPointer]) != 0)
	{
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

		if (ModRMExtesionTable[aImage[aExecutionPointer]].count(byte.reg) != 0)
		{
			return ModRMExtesionTable[aImage[aExecutionPointer]][byte.reg](aImage, aExecutionPointer + 1, aSections, aREX, aInstructionBase);
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

	std::map<unsigned char, Instruction>::const_iterator it = aOpcodeTable.find(nextInstruction);

	if (it != aOpcodeTable.cend())
	{
		return (it->second)(aImage, aExecutionPointer + 1, aSections, aREX, aInstructionBase);
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

		return (it->second)(aImage, aExecutionPointer + 1, aSections, rex, aInstructionBase);
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
	size_t										aInstructionBase)
{
	std::map<unsigned char, Instruction> secondaryOpcodeTable;

	secondaryOpcodeTable[0xb6]		= MOVZX;
	secondaryOpcodeTable[0xb7]		= MOVZX;
	secondaryOpcodeTable[0xb0]		= CMPXCHG;
	secondaryOpcodeTable[0xb1]		= CMPXCHG;

	unsigned char nextInstruction	= aImage[aExecutionPointer + 1];

	if (secondaryOpcodeTable.count(nextInstruction) != 0)
	{
		return secondaryOpcodeTable[nextInstruction](aImage, aExecutionPointer + 1, aSections, aREX, aInstructionBase);
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

	std::map<unsigned char, Instruction> legacyOpCodeTable;

	legacyOpCodeTable[0x0f] = SecondaryOPCodeTable;

	for (unsigned char	i   = 0x00; i <= 0x05; i++)
		legacyOpCodeTable[i] = ADD;

	for (unsigned char	i   = 0x08; i <= 0x0D; i++)
		legacyOpCodeTable[i] = OR;

	for (unsigned char	i   = 0x30; i <= 0x35; i++)
		legacyOpCodeTable[i] = XOR;

	for (unsigned char	i   = 0x38; i <= 0x3d; i++)
		legacyOpCodeTable[i] = CMP;

	for (unsigned char	i   = 0x40; i <= 0x4F; i++)
		legacyOpCodeTable[i] = std::bind(REXPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));

	for (unsigned char	i   = 0x50; i <= 0x57; i++)
		legacyOpCodeTable[i] = PUSH;
	
	legacyOpCodeTable[0x2e] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0x3e] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0x26] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0x64] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0x65] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));
	legacyOpCodeTable[0x36] = std::bind(LegacyPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));

	legacyOpCodeTable[0x74] = JCC;
	legacyOpCodeTable[0x75] = JCC;

	legacyOpCodeTable[0x80] = ModRMExtension;
	legacyOpCodeTable[0x81] = ModRMExtension;
	legacyOpCodeTable[0x82] = ModRMExtension;
	legacyOpCodeTable[0x83] = ModRMExtension;

	legacyOpCodeTable[0x85] = TEST;

	for (unsigned char	i   = 0x88; i <= 0x8c; i++)
		legacyOpCodeTable[i] = MOV;

	legacyOpCodeTable[0x8d] = LEA;

	legacyOpCodeTable[0xa4] = MOVS;
	legacyOpCodeTable[0xa5] = MOVS;

	for (unsigned char	i   = 0xb0; i <= 0xbF; i++)
		legacyOpCodeTable[i] = MOV;

	legacyOpCodeTable[0xc2] = RET;
	legacyOpCodeTable[0xc3] = RET;

	legacyOpCodeTable[0xc6] = ModRMExtension;
	legacyOpCodeTable[0xc7] = ModRMExtension;
	legacyOpCodeTable[0xCD] = INT_Instruction;

	legacyOpCodeTable[0xF6] = ModRMExtension;
	legacyOpCodeTable[0xF7] = ModRMExtension;

	legacyOpCodeTable[0xE8] = CALL_near_32_im;
	legacyOpCodeTable[0xE9] = JMPNear;
	legacyOpCodeTable[0xEB] = JMPNear;


	legacyOpCodeTable[0xF0] = CLC;
	legacyOpCodeTable[0xFF] = ModRMExtension;

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
			for (size_t			i = 0; i < globals::globalBranches.size(); i++)
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
			for (const Symbol&	symbol : aSymbols)
			{
				if (symbol.myAddress == executionPointer)
				{
					isSymbol = true;
					printf("\nImported %s symbol: [%s] ", symbol.myType.c_str(), symbol.mySource.c_str());
					PRINTF_PURPLE("%s\n\n", aImage.data() + symbol.myName);
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
			for (const IMAGE_SECTION_HEADER& section : aSections)
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
				executionPointer = legacyOpCodeTable[aImage[executionPointer]](aImage, executionPointer, aSections, rex, executionPointer);
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
}