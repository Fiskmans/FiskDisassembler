#include "ModRMExtended.h"

#include "Printers.h"

size_t
JMP_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("JMP ");
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	size_t		out	   = -1;

	switch (byte.reg)
	{
	case 0x4: {

		size_t loc = 0;
		if (aIs16Bit)
			out = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1, false, &loc);
		else if (aREX.w)
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
TEST_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)

{
	printf("TEST ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);
	switch (aImage[aExecutionPointer - 1])
	{
	case 0xF6:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xF7:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM64, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("ERROR");
	return -1;
}

size_t
NOT_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("NOT ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);
	switch (aImage[aExecutionPointer - 1])
	{
	case 0xF6:
		return Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xF7:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
	}
}

size_t
INC_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("INC ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xFE:
		return Operands(OperandType::REG8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xFF:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REG32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("ERROR");
	return -1;
}

size_t
DEC_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("DEC ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xFE:
		return Operands(OperandType::REG8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xFF:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REG32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("ERROR");
	return -1;
}

size_t
CALL_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase,
	size_t										aImageBase)
{
	printf("CALL ");
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	size_t		out	   = -1;

	switch (byte.reg)
	{
	case 0x2: {
		uint64_t loc = 0;
		if (aIs16Bit)
			out = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1, false, &loc);
		else
			out = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1, false, &loc);

		if (loc >= aImageBase)
		{
			loc -= aImageBase;
		}

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

		return out;
	}
	}

	PRINTF_RED("ERROR");
	return -1;
}

size_t
MOV_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("MOV ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xc6:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xc7:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	}

	printf("unkown mov_rm type");
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
ROL_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	size_t		end;
	ModRMByte	byte = ParseModRM(aImage[aExecutionPointer], aREX);
	printf("ROL ");

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xc0:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xc1:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xd0:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd1:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd2:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	case 0xd3:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	}

	PRINTF_RED("Unkown ROL_rm");
	return -1;
}

size_t
ROR_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	size_t		end;
	ModRMByte	byte = ParseModRM(aImage[aExecutionPointer], aREX);
	printf("ROR ");

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xc0:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xc1:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xd0:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd1:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd2:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	case 0xd3:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	}

	PRINTF_RED("Unkown ROR_rm");
	return -1;
}

size_t
SAL_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	size_t		end	   = 0;
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	printf("SAL ");

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xc0:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xc1:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xd0:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd1:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd2:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	case 0xd3:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	}

	PRINTF_RED("Unkown SAL_rm");
	return -1;
}

size_t
SHR_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	size_t		end	   = 0;
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	printf("SHR ");

	switch (aImage[aExecutionPointer - 1])
	{
	case 0xc0:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xc1:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0xd0:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd1:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", 1");
		return end;
	case 0xd2:
		end = Operands(OperandType::REGMEM8, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	case 0xd3:
		if (aIs16Bit)
			end = Operands(OperandType::REGMEM16, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else if (aREX.w)
			end = Operands(OperandType::REGMEM64, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		else
			end = Operands(OperandType::REGMEM32, OperandType::NONE, aREX, byte, aImage, aExecutionPointer + 1);
		printf(", [CL]");
		return end;
	}

	PRINTF_RED("Unkown SHR_rm");
	return -1;
}

size_t
ADD_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
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
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x83:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("Unkown add_rm");
	return -1;
}

size_t
OR_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("OR ");
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x80:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x81:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x83:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("Unkown or_rm");
	return -1;
}

size_t
AND_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("AND ");
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x80:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x81:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x83:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("Unkown and_rm");
	return -1;
}

size_t
SUB_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("SUB ");
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x80:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x81:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x83:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("Unkown sub_rm");
	return -1;
}

size_t
XOR_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("XOR ");
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x80:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x81:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x83:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("Unkown xor_rm");
	return -1;
}

size_t
CMP_RM(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("CMP ");
	switch (aImage[aExecutionPointer - 1])
	{
	case 0x80:
		return Operands(OperandType::REGMEM8, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x81:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM16, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM32, aREX, byte, aImage, aExecutionPointer + 1);
	case 0x83:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
		return Operands(OperandType::REGMEM32, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	}

	PRINTF_RED("Unkown cmp_rm");
	return -1;
}