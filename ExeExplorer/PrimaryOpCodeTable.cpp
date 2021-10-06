#include "PrimaryOpCodeTable.h"

#include "ConsoleHelp.h"
#include "Printers.h"

size_t
ADD(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	printf("ADD ");
	switch ((aImage[aExecutionPointer] & 0x0F) % 8)
	{
	case 0x00:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x01:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x02:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x03:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x04: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
		printf("[AL], ");
		PRINTF_GREEN("$0x%02x", imm);

		return aExecutionPointer + 1 + sizeof(imm);
	}
	case 0x05: {
		if (aIs16Bit)
		{
			uint16_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[AX], ");
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else if (aREX.w)
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[RAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[EAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
	}
	}

	PRINTF_RED("unkown add type");
	return -1;
}

size_t
AND(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	printf("AND ");
	switch ((aImage[aExecutionPointer] & 0x0F) % 8)
	{
	case 0x00:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x01:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x02:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x03:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x04: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
		printf("[AL], ");
		PRINTF_GREEN("$0x%02x", imm);

		return aExecutionPointer + 1 + sizeof(imm);
	}
	case 0x05: {
		if (aIs16Bit)
		{
			uint16_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[AX], ");
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else if (aREX.w)
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[RAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[EAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
	}
	}

	PRINTF_RED("unkown and type");
	return -1;
}

size_t
OR(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	printf("OR ");
	switch ((aImage[aExecutionPointer] & 0x0F) % 8)
	{
	case 0x00:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x01:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x02:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x03:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x04: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
		printf("[AL], ");
		PRINTF_GREEN("$0x%02x", imm);

		return aExecutionPointer + 1 + sizeof(imm);
	}
	case 0x05: {
		if (aIs16Bit)
		{
			uint16_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[AX], ");
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else if (aREX.w)
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[RAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[EAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
	}
	}

	PRINTF_RED("unkown or type");
	return -1;
}

size_t
SUB(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	printf("SUB ");
	switch ((aImage[aExecutionPointer] & 0x0F) % 8)
	{
	case 0x00:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x01:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x02:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x03:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x04: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
		printf("[AL], ");
		PRINTF_GREEN("$0x%02x", imm);

		return aExecutionPointer + 1 + sizeof(imm);
	}
	case 0x05: {
		if (aIs16Bit)
		{
			uint16_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[AX], ");
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else if (aREX.w)
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[RAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[EAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
	}
	}

	PRINTF_RED("unkown sub type");
	return -1;
}

size_t
CMP(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("CMP ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
	switch ((aImage[aExecutionPointer] & 0x0F) % 8)
	{
	case 0x00:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x01:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x02:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x03:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x04: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
		printf("[AL], ");
		PRINTF_GREEN("$0x%02x", imm);

		return aExecutionPointer + 1 + sizeof(imm);
	}
	case 0x05: {
		if (aIs16Bit)
		{
			uint16_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[AX], ");
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else if (aREX.w)
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[RAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[EAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
	}
	}

	PRINTF_RED("unmapped cmp opcode %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
XOR(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("XOR ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
	switch ((aImage[aExecutionPointer] & 0x0F) % 8)
	{
	case 0x00:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x01:
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x02:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x03:
		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x04: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
		printf("[AL], ");
		PRINTF_GREEN("$0x%02x", imm);

		return aExecutionPointer + 1 + sizeof(imm);
	}
	case 0x05: {
		if (aIs16Bit)
		{
			uint16_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[AX], ");
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else if (aREX.w)
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[RAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
		else
		{
			uint32_t imm;
			memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
			printf("[EAX], ");
			PRINTF_GREEN("$0x%08x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
	}
	}

	PRINTF_RED("unkown xor");
	PrintAround(aImage, aInstructionBase);
	return -1;
}

size_t
IMUL(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("IMUL ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
	switch (aImage[aExecutionPointer])
	{
	case 0x6b: {
		size_t end;
		if (aIs16Bit)
			end = Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);
		else if (aREX.w)
			end = Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);
		else
			end = Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);

		uint8_t imm;
		memcpy(&imm, aImage.data() + end, sizeof(imm));

		printf(", ");
		PRINTF_GREEN("$0x%02x", imm);

		return end + sizeof(imm);
	}
	case 0x69: {
		size_t end;
		if (aIs16Bit)
			end = Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);
		else if (aREX.w)
			end = Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);
		else
			end = Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);

		uint16_t imm;
		memcpy(&imm, aImage.data() + end, sizeof(imm));

		printf(", ");
		PRINTF_GREEN("$0x%04x", imm);

		return end + sizeof(imm);
	}
	}

	PRINTF_RED("Unkown IMUL type");
	PrintAround(aImage, aInstructionBase);
	return -1;
}

size_t
JCC(
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

size_t
XCHG(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("XCHG ");

	switch (aImage[aExecutionPointer])
	{
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
		if (aIs16Bit)
			printf("[AX], [");
		if (aREX.w)
			printf("[RAX], [R");
		else
			printf("[EAX], [E");

		switch (aImage[aExecutionPointer] & 0x0f)
		{
		case 0x00:
			printf("AX]");
		case 0x01:
			printf("DX]");
		case 0x02:
			printf("CX]");
		case 0x03:
			printf("BX]");
		case 0x04:
			printf("SP]");
		case 0x05:
			printf("BP]");
		case 0x06:
			printf("SI]");
		case 0x07:
			printf("DI]");
		}
		return aExecutionPointer + 1;

	case 0x86: {
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
		return Operands(OperandType::REGMEM8, OperandType::REG8, aREX, byte, aImage, aExecutionPointer + 2);
	}
	case 0x87: {
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	}
	}
	printf("unmapped xchg %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
TEST(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("TEST ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
	switch (aImage[aExecutionPointer])
	{
	case 0x84:
		return Operands(OperandType::REGMEM8, OperandType::REG8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x85:

		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	}

	printf("unmapped test opcode %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
LEA(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("LEA ");

	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	if (aIs16Bit)
		return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2, true);

	if (aREX.w)
		return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2, true);

	return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2, true);
}

size_t
CBW(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("CBW");
	return aExecutionPointer + 1;
}

size_t
MOVS(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{

	switch (aImage[aExecutionPointer])
	{
	case 0xa4:
		printf("MOVS [rSI], [rDI]");
		break;
	case 0xa5:
		if (aIs16Bit)
			printf("MOVS [rSI], [rDI]");
		else if (aREX.w)
			printf("MOVS [rSI], [rDI]");
		else
			printf("MOVS [rSI], [rDI]");

		break;
	}
	return aExecutionPointer + 1;
}

size_t
RET(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
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
INT3(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("INT3");
	return aExecutionPointer + 1;
}

size_t
CALL(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("CALL ");
	size_t	nextInstruction;
	int32_t distance;

	if (aIs16Bit)
	{
		int16_t offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));
		nextInstruction = aExecutionPointer + 1 + sizeof(offset);
		PrintSignedHex(offset);
		distance		= offset;
	}
	else
	{
		int32_t offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));
		nextInstruction = aExecutionPointer + 1 + sizeof(offset);
		PrintSignedHex(offset);
		distance		= offset;
	}

	size_t		target  = nextInstruction + distance;
	std::string name	= GetOrGenerateFunctionName(target);

	AddFunction(target);

	SetColumn(globals::globalLocationColumn);
	printf("0x%08zx  ", target);
	PrintFunction(target);

	return nextInstruction;
}

size_t
JMP(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("JMP ");
	switch (aImage[aExecutionPointer])
	{
	case 0xe9: {
		int32_t offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));

		PrintSignedHex(offset);
		size_t target = aExecutionPointer + 1 + sizeof(offset) + offset;
		AddBranch(target);

		SetColumn(globals::globalLocationColumn);
		printf("0x%08zx  ", target);
		PrintLocation(target);
		return -1;
	}
	case 0xEB: {
		int8_t offset;
		memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));

		PrintSignedHex(offset);
		size_t target = aExecutionPointer + 1 + sizeof(offset) + offset;
		AddBranch(target);

		SetColumn(globals::globalLocationColumn);
		printf("0x%08zx  ", target);
		PrintLocation(target);
		return -1;
	}
	}

	PRINTF_RED("unkown mov");
	PrintAround(aImage, aInstructionBase);
	return -1;
}

size_t
CLC(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("CLC");
	return aExecutionPointer + 1;
}

size_t
MOVSXD(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("MOVSXD ");
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
	return Operands(OperandType::REG64, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
}

size_t
STOS(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	switch (aImage[aExecutionPointer])
	{
	case 0xAA:
		printf("STOSB");
		return aExecutionPointer + 1;
	case 0xAB:
		if (aIs16Bit)
			printf("STOSW");
		if (aREX.w)
			printf("STOSQ");
		else
			printf("STOSD");

		return aExecutionPointer + 1;
	}
	PRINTF_RED("Unkown stos");
	return -1;
}
size_t
MOV(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("MOV ");

	switch (aImage[aExecutionPointer])
	{
	case 0x88: {
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);
		return Operands(OperandType::REGMEM8, OperandType::REG8, aREX, byte, aImage, aExecutionPointer + 2);
	}
	case 0x89: {
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

		if (aIs16Bit)
			return Operands(OperandType::REGMEM16, OperandType::REG16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	}

	case 0x8B: {
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

		if (aIs16Bit)
			return Operands(OperandType::REG16, OperandType::REGMEM16, aREX, byte, aImage, aExecutionPointer + 2);

		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	}

	break;
	case 0xb0:
	case 0xb1:
	case 0xb2:
	case 0xb3:

		switch ((aImage[aExecutionPointer] & 0x0F))
		{
		case 0x0:
			printf("[AL]");
			break;
		case 0x1:
			printf("[CL]");
			break;
		case 0x2:
			printf("[DL]");
			break;
		case 0x3:
			printf("[BL]");
			break;
		}
		{
			uint8_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf(", ");
			PRINTF_GREEN("$0x%02x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
		break;
	case 0xb4:
	case 0xb5:
	case 0xb6:
	case 0xb7:
		switch ((aImage[aExecutionPointer] & 0x0F) >> 2)
		{
		case 0x0:
			printf("[AH]");
			break;
		case 0x1:
			printf("[CH]");
			break;
		case 0x2:
			printf("[DH]");
			break;
		case 0x3:
			printf("[BH]");
			break;
		}
		{
			uint8_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf(", ");
			PRINTF_GREEN("$0x%02x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
		break;
		break;
	case 0xb8:
	case 0xb9:
	case 0xba:
	case 0xbb:
	case 0xbc:
	case 0xbd:
	case 0xbe:
	case 0xbf: {
		switch ((aImage[aExecutionPointer] & 0x0F) >> 4)
		{
		case 0x0:
			printf("[rAX]");
			break;
		case 0x1:
			printf("[rCX]");
			break;
		case 0x2:
			printf("[rDX]");
			break;
		case 0x3:
			printf("[rBX]");
			break;
		case 0x4:
			printf("[rSP]");
			break;
		case 0x5:
			printf("[rBP]");
			break;
		case 0x6:
			printf("[rSI]");
			break;
		case 0x7:
			printf("[rDI]");
			break;
		}

		if (aREX.w)
		{
			uint64_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf(", ");
			PRINTF_GREEN("$0x%016I64x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
		else
		{
			uint32_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf(", ");
			PRINTF_GREEN("$0x%08x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
	}
	break;
	}

	printf("unkown mov type");
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
PUSH(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("PUSH ");

	switch (aImage[aExecutionPointer])
	{
	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57: {
		switch ((aImage[aExecutionPointer] & 0x0F))
		{
		case 0x0:
			printf("[rAX]");
			break;
		case 0x1:
			printf("[rCX]");
			break;
		case 0x2:
			printf("[rDX]");
			break;
		case 0x3:
			printf("[rBX]");
			break;
		case 0x4:
			printf("[rSP]");
			break;
		case 0x5:
			printf("[rBP]");
			break;
		case 0x6:
			printf("[rSI]");
			break;
		case 0x7:
			printf("[rDI]");
			break;
		}
	}
		return aExecutionPointer + 1;
	}

	printf("unkown push type");
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
POP(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("POP ");

	switch (aImage[aExecutionPointer])
	{
	case 0x58:
	case 0x59:
	case 0x5a:
	case 0x5b:
	case 0x5c:
	case 0x5d:
	case 0x5e:
	case 0x5f: {
		switch ((aImage[aExecutionPointer] & 0x0F) >> 4)
		{
		case 0x0:
			printf("[rAX]");
			break;
		case 0x1:
			printf("[rCX]");
			break;
		case 0x2:
			printf("[rDX]");
			break;
		case 0x3:
			printf("[rBX]");
			break;
		case 0x4:
			printf("[rSP]");
			break;
		case 0x5:
			printf("[rBP]");
			break;
		case 0x6:
			printf("[rSI]");
			break;
		case 0x7:
			printf("[rDI]");
			break;
		}
	}
		return aExecutionPointer + 1;
	}

	printf("unkown pop type");
	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
}

size_t
INT_Instruction(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase)
{
	printf("INT ");
	uint8_t data;
	memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
	PRINTF_GREEN("$0x%02x", data);
	return aExecutionPointer + 2;
}