#include "SecondaryOpCodeTable.h"

#include "Printers.h"

size_t
CPUID_sec(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("CPUID");
	return aExecutionPointer + 1;
}

size_t
JCC_sec(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{

	switch (aImage[aExecutionPointer] & 0x0f)
	{
	case 0x00:
		printf("JO ");
		break;
	case 0x01:
		printf("JNO ");
		break;
	case 0x02:
		printf("JC ");
		break;
	case 0x03:
		printf("JNB ");
		break;
	case 0x04:
		printf("JZ  ");
		break;
	case 0x05:
		printf("JNZ ");
		break;
	case 0x06:
		printf("JBE ");
		break;
	case 0x07:
		printf("JNBE ");
		break;
	case 0x08:
		printf("JS ");
		break;
	case 0x09:
		printf("JNS ");
		break;
	case 0x0a:
		printf("JP ");
		break;
	case 0x0b:
		printf("JNP ");
		break;
	case 0x0c:
		printf("JL ");
		break;
	case 0x0d:
		printf("JNL ");
		break;
	case 0x0e:
		printf("JLE ");
		break;
	case 0x0f:
		printf("JG ");
		break;
	default:
		printf("unkown jcc type");
		PrintAround(aImage, aExecutionPointer);

		return -1;
	}
	size_t	dest;
	size_t	end;

	if (aIs16Bit)
	{
		int16_t offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));
		dest	= aExecutionPointer + 1 + sizeof(offset) + offset;
		end		= aExecutionPointer + sizeof(offset) + 1;
		PrintSignedHex(offset);
	}
	else
	{
		int32_t offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));
		dest	= aExecutionPointer + 1 + sizeof(offset) + offset;
		end		= aExecutionPointer + sizeof(offset) + 1;
		PrintSignedHex(offset);
	}

	AddBranch(dest);

	SetColumn(globals::globalLocationColumn);
	printf("0x%08zx  ", dest);
	PrintLocation(dest);

	return end;
}

size_t
XGETSETBV_sec_rm(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer], aREX);
	switch (byte.rm)
	{
	case 0x00:
		printf("XGETBV");
		return aExecutionPointer + 1;
	case 0x01:
		printf("XSETBV");
		return aExecutionPointer + 1;
	default:
		PRINTF_RED("INVALID CODE")
		return -1;
	}
}

size_t
CMPXCHG(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("CMPXCHG ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	switch (aImage[aExecutionPointer])
	{
	case 0xb0:
		return Operands(OperandType::REGMEM8, OperandType::REG8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0xb1:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	}

	PRINTF_RED("unkown cmpxchg");
	return -1;
}

size_t
MOVZX(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("MOVZX ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	switch (aImage[aExecutionPointer])
	{
	case 0xB6: {
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);

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
NOP_sec(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("NOP ");
	return aExecutionPointer + 2;
}