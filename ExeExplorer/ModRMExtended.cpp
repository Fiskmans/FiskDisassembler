#include "ModRMExtended.h"

#include "Printers.h"

size_t
JMP_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("JMP ");
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	size_t		out	   = -1;

	switch (byte.reg)
	{
	case 0x4: {

		size_t loc = 0;
		if (aREX.w)
			out = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1, false, &loc);
		else
			out = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1, false, &loc);

		if (loc != 0)
		{
			AddBranch(loc, "_memaddr");
			printf("\t");
			PrintLocation(loc);
		}
		else
		{
			PrintLocation(GenerateName("location_", "_regaddr"));
		}
	}
	break;
	default:
		break;
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
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	}

	printf("unkown mov_rm type");
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

	PRINTF_RED("unmapped cmp_rm opcode %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}