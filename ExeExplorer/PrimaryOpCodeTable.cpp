#include "PrimaryOpCodeTable.h"

#include "ConsoleHelp.h"
#include "Printers.h"

size_t
ADD(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	printf("ADD ");
	switch (aImage[aExecutionPointer])
	{
	case 0x00:
		return Operands(OperandType::REGMEM8, OperandType::REG8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x01:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x02:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x03:
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
		if (aREX.w)
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
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}

	}
	}

	PRINTF_RED("unkown add type");
	return -1;
}

size_t
OR(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

	printf("OR ");
	switch (aImage[aExecutionPointer])
	{
	case 0x08:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x09:
		if (aREX.w)
			return Operands(OperandType::REGMEM64, OperandType::REG64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

	case 0x0a:
		return Operands(OperandType::REG8, OperandType::REGMEM8, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x0b:
		if (aREX.w)
			return Operands(OperandType::REG64, OperandType::REGMEM64, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);
	case 0x0c: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + aExecutionPointer + 1, sizeof(imm));
		printf("[AL], ");
		PRINTF_GREEN("$0x%02x", imm);

		return aExecutionPointer + 1 + sizeof(imm);
	}
	case 0x0d: {
		if (aREX.w)
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
			PRINTF_GREEN("$0x%04x", imm);
			return aExecutionPointer + 1 + sizeof(imm);
		}
	}
	}

	PRINTF_RED("unkown add type");
	return -1;
}

size_t
JMPNear(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
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
MOV(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
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

		if (aREX.w)
			return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);

		return Operands(OperandType::REGMEM32, OperandType::REG32, aREX, byte, aImage, aExecutionPointer + 2);
	}

	case 0x8B: {
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

		if (aREX.w)
			return Operands(OperandType::REG32, OperandType::REGMEM32, aREX, byte, aImage, aExecutionPointer + 2);

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
INT_Instruction(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("INT ");
	uint8_t data;
	memcpy(&data,aImage.data() + aExecutionPointer + 1, sizeof(data));
	PRINTF_GREEN("$0x%02x",data);
	return aExecutionPointer + 2;
}