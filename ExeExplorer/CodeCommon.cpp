#include "CodeCommon.h"
#include "ConsoleHelp.h"
#include <algorithm>
#include <map>

namespace globals {
	std::vector<BranchType> globalFunctions;
	std::vector<BranchType> globalBranches;
	const size_t			globalLocationColumn	 = 50;
	size_t					globalImageBase		   = 0;
} // globals

std::string
GenerateName(
	std::string							aName,
	std::string							aSuffix)
{
	static size_t counter = 0;
	return aName + std::to_string(++counter) + aSuffix;
}

std::string
GetOrGenerateFunctionName(
	size_t								aAddress,
	std::string							aSuffix)
{
	static std::map<size_t, std::string> names;
	if (names.count(aAddress) == 0)
	{
		names[aAddress] = GenerateName("function_", aSuffix);

		for (BranchType& b : globals::globalFunctions)
		{
			if (b.myAddress == aAddress)
			{
				names[aAddress] = b.myName;
			}
		}
	}

	return names[aAddress];
}

std::string
GetOrGenerateLocationName(
	size_t								aAddress,
	std::string							aSuffix)
{
	static std::map<size_t, std::string> names;
	if (names.count(aAddress) == 0)
	{
		names[aAddress] = GenerateName("location_", aSuffix);
	}

	return names[aAddress];
}

void
AddFunction(
	BranchType							aFunction)
{
	globals::globalFunctions.insert(
		std::upper_bound(
			globals::globalFunctions.begin(),
			globals::globalFunctions.end(),
			aFunction,
			[](const BranchType& aA, const BranchType& aB) { return aA.myAddress < aB.myAddress; }),
		aFunction);
}

void
AddFunction(
	size_t								aAddress,
	std::string							aSuffix)
{
	for (BranchType& b : globals::globalFunctions)
		if (b.myAddress == aAddress)
			return;

	AddFunction({GetOrGenerateFunctionName(aAddress, aSuffix), aAddress, false});
}

void
AddBranch(
	size_t								aAddress,
	std::string							aSuffix)
{
	for (BranchType& b : globals::globalBranches)
		if (b.myAddress == aAddress)
			return;

	BranchType b{GetOrGenerateLocationName(aAddress, aSuffix), aAddress, false};

	globals::globalBranches.insert(
		std::upper_bound(
			globals::globalBranches.begin(),
			globals::globalBranches.end(),
			b,
			[](const BranchType& aA, const BranchType& aB) { return aA.myAddress < aB.myAddress; }),
		b);
}

void
PrintFunction(
	const std::string&					aName)
{
	PRINTF_LIGHTBLUE("%s", aName.c_str());
}
void
PrintFunction(
	size_t								aAddress)
{
	PrintFunction(GetOrGenerateFunctionName(aAddress));
}

void
PrintLocation(
	const std::string&					aName)
{
	PRINTF_BLUE("%s", aName.c_str());
}

void
PrintLocation(
	size_t								aAddress)
{
	PrintLocation(GetOrGenerateLocationName(aAddress));
}

ModRMByte
ParseModRM(
	unsigned char						aByte,
	REXState							aREX)
{
	ModRMByte ret;
	ret.mod = (aByte & 0b11000000) >> 6;
	ret.reg = (aByte & 0b00111000) >> 3;
	ret.rm	= (aByte & 0b00000111);

	if (aREX.r)
		ret.reg |= 0b1000;

	if (aREX.b)
		ret.rm |= 0b1000;

	return ret;
}

SIBByte
ParseSIB(
	unsigned char						aByte,
	REXState							aREX)
{
	SIBByte ret;
	ret.scale	= (aByte & 0b11000000) >> 6;
	ret.index	= (aByte & 0b00111000) >> 3;
	ret.base	= (aByte & 0b00000111);

	if (aREX.x)
		ret.index |= 0b1000;

	if (aREX.b)
		ret.base |= 0b1000;

	return ret;
}

std::string
RegMemSIB(
	ModRMByte							aModRM,
	SIBByte								aSIB)
{
	std::string out = "[";
	switch (aSIB.index)
	{
	case 0b000:
		out += (std::to_string(1 << aSIB.scale)) + " * rAX + ";
		break;
	case 0b001:
		out += (std::to_string(1 << aSIB.scale)) + " * rCX + ";
		break;
	case 0b010:
		out += (std::to_string(1 << aSIB.scale)) + " * rDX + ";
		break;
	case 0b011:
		out += (std::to_string(1 << aSIB.scale)) + " * rBX + ";
		break;
	case 0b100:
		break;
	case 0b101:
		out += (std::to_string(1 << aSIB.scale)) + " * rBP + ";
		break;
	case 0b110:
		out += (std::to_string(1 << aSIB.scale)) + " * rSI + ";
		break;
	case 0b111:
		out += (std::to_string(1 << aSIB.scale)) + " * rDI + ";
		break;
	}
	switch (aSIB.base)
	{
	case 0b000:
		out += "rAX + ";
		break;
	case 0b001:
		out += "rCX + ";
		break;
	case 0b010:
		out += "rDX + ";
		break;
	case 0b011:
		out += "rBX + ";
		break;
	case 0b100:
		out += "rSP + ";
		break;
	case 0b101:
		if (aModRM.mod != 0b00)
		{
			out += "rBP + ";
		}
		break;
	case 0b110:
		out += "rSI + ";
		break;
	case 0b111:
		out += "rDI + ";
		break;
	}
	out += "offset]";

	return out;
}

std::string
RegMem(
	ModRMByte							aModRM,
	REXState							aREX,
	RegisterSize						aRegisterSize,
	bool								aSelector,
	const std::vector<unsigned char>&	aImage,
	size_t								aNextByte,
	uint32_t&							aOutExtraConsumed,
	int32_t*							aOutMarkerAt)
{
	aOutExtraConsumed = 0;

	if (aSelector)
	{
		switch (aModRM.reg)
		{
		case 0b0000:
			return "[rAX]";
		case 0b0001:
			return "[rCX]";
		case 0b0010:
			return "[rDX]";
		case 0b0011:
			return "[rBX]";
		case 0b0100:
			return "[rSP]";
		case 0b0101:
			return "[rBP]";
		case 0b0110:
			return "[rSI]";
		case 0b0111:
			return "[rDI]";
		case 0b1000:
			return "[r8]";
		case 0b1001:
			return "[r9]";
		case 0b1010:
			return "[r10]";
		case 0b1011:
			return "[r11]";
		case 0b1100:
			return "[r12]";
		case 0b1101:
			return "[r13]";
		case 0b1110:
			return "[r14]";
		case 0b1111:
			return "[r15]";
		}
		return "[unkwon reg field]";
	}

	switch (aModRM.mod)
	{
	case 0b11:
		switch (aModRM.rm)
		{
		case 0b0000:
			return "[rAX]";
		case 0b0001:
			return "[rCX]";
		case 0b0010:
			return "[rDX]";
		case 0b0011:
			return "[rBX]";
		case 0b0100:
			return "[rSP]";
		case 0b0101:
			return "[rBP]";
		case 0b0110:
			return "[rSI]";
		case 0b0111:
			return "[rDI]";
		case 0b1000:
			return "[r8]";
		case 0b1001:
			return "[r9]";
		case 0b1010:
			return "[r10]";
		case 0b1011:
			return "[r11]";
		case 0b1100:
			return "[r12]";
		case 0b1101:
			return "[r13]";
		case 0b1110:
			return "[r14]";
		case 0b1111:
			return "[r15]";
		}
		break;
	case 0b01: {
		int8_t offset;
		memcpy(&offset, aImage.data() + aNextByte, sizeof(offset));
		aOutExtraConsumed = 1;
		switch (aModRM.rm)
		{
		case 0b000:
			return "[rAX" + StringSignedHex(offset) + "]";
		case 0b001:
			return "[rCX" + StringSignedHex(offset) + "]";
		case 0b010:
			return "[rDX" + StringSignedHex(offset) + "]";
		case 0b011:
			return "[rBX" + StringSignedHex(offset) + "]";
		case 0b100: {
			SIBByte		sib = ParseSIB(aImage[aNextByte], aREX);

			std::string out = "[";
			switch (sib.base)
			{
			case 0b000:
				out += "rAX";
				break;
			case 0b001:
				out += "rCX";
				break;
			case 0b010:
				out += "rDX";
				break;
			case 0b011:
				out += "rBX";
				break;
			case 0b100:
				out += "rSP";
				break;
			case 0b101:
				out += "rBP";
				break;
			case 0b110:
				out += "rSI";
				break;
			case 0b111:
				out += "rDI";
				break;
			}
			int8_t offsetWithSib;
			memcpy(&offsetWithSib, aImage.data() + aNextByte + 1, sizeof(offsetWithSib));
			out += StringSignedHex(offset) + "]";
			aOutExtraConsumed = 2;

			return out;
		}
		case 0b101:
			return "[how ??" + StringSignedHex(offset) + "]";
		case 0b110: {
			return "[rSI" + StringSignedHex(offset) + "]";
		}
		case 0b111:
			return "[rDI" + StringSignedHex(offset) + "]";
		case 0b1000:
			return "[r8" + StringSignedHex(offset) + "]";
		case 0b1001:
			return "[r9" + StringSignedHex(offset) + "]";
		case 0b1010:
			return "[r10" + StringSignedHex(offset) + "]";
		case 0b1011:
			return "[r11" + StringSignedHex(offset) + "]";
		case 0b1100:
			return "[r12" + StringSignedHex(offset) + "]";
		case 0b1101:
			return "[r13" + StringSignedHex(offset) + "]";
		case 0b1110:
			return "[r14" + StringSignedHex(offset) + "]";
		case 0b1111:
			return "[r15" + StringSignedHex(offset) + "]";
		}
		break;
	}
	case 0b10: {
		int32_t offset;
		memcpy(&offset, aImage.data() + aNextByte, sizeof(offset));
		aOutExtraConsumed = 4;
		switch (aModRM.rm)
		{
		case 0b000:
			return "[rAX" + StringSignedHex(offset) + "]";
		case 0b001:
			return "[rCX" + StringSignedHex(offset) + "]";
		case 0b010:
			return "[rDX" + StringSignedHex(offset) + "]";
		case 0b011:
			return "[rBX" + StringSignedHex(offset) + "]";
		case 0b100: {
			SIBByte		sib = ParseSIB(aImage[aNextByte], aREX);

			std::string out = "[";
			switch (sib.base)
			{
			case 0b000:
				out += "rAX";
				break;
			case 0b001:
				out += "rCX";
				break;
			case 0b010:
				out += "rDX";
				break;
			case 0b011:
				out += "rBX";
				break;
			case 0b100:
				out += "rSP";
				break;
			case 0b101:
				out += "rBP";
				break;
			case 0b110:
				out += "rSI";
				break;
			case 0b111:
				out += "rDI";
				break;
			}
			int32_t offsetWithSib;
			memcpy(&offsetWithSib, aImage.data() + aNextByte + 1, sizeof(offsetWithSib));
			out += StringSignedHex(offset) + "]";
			aOutExtraConsumed = 5;

			return out;
		}
		case 0b101:
			return "[how ??" + StringSignedHex(offset) + "]";
		case 0b110: {
			return "[rSI" + StringSignedHex(offset) + "]";
		}
		case 0b111:
			return "[rDI" + StringSignedHex(offset) + "]";
		case 0b1000:
			return "[r8" + StringSignedHex(offset) + "]";
		case 0b1001:
			return "[r9" + StringSignedHex(offset) + "]";
		case 0b1010:
			return "[r10" + StringSignedHex(offset) + "]";
		case 0b1011:
			return "[r11" + StringSignedHex(offset) + "]";
		case 0b1100:
			return "[r12" + StringSignedHex(offset) + "]";
		case 0b1101:
			return "[r13" + StringSignedHex(offset) + "]";
		case 0b1110:
			return "[r14" + StringSignedHex(offset) + "]";
		case 0b1111:
			return "[r15" + StringSignedHex(offset) + "]";
		}
		break;
	}
	case 0b00:
		switch (aModRM.rm)
		{
		case 0b000:
			return "[rAX]";
		case 0b001:
			return "[rCX]";
		case 0b010:
			return "[rDX]";
		case 0b011:
			return "[rBX]";
		case 0b100: {
			aOutExtraConsumed	= 1;

			SIBByte sib			= ParseSIB(aImage[aNextByte], aREX);

			return RegMemSIB(aModRM, sib);
		}
		case 0b101: {
			std::string out = "[rIP ";
			int32_t		offset;
			memcpy(&offset, aImage.data() + aNextByte, sizeof(offset));
			aOutExtraConsumed = sizeof(offset);
			out += StringSignedHex(offset);
			if (aOutMarkerAt)
			{
				*aOutMarkerAt = offset;
			}

			return out + "]";
		}
		case 0b110:
			return "[rSI]";
		case 0b111:
			return "[rDI]";
		case 0b1000:
			return "[r8]";
		case 0b1001:
			return "[r9]";
		case 0b1010:
			return "[r10]";
		case 0b1011:
			return "[r11]";
		case 0b1100:
			return "[r12]";
		case 0b1101:
			return "[r13]";
		case 0b1110:
			return "[r14]";
		case 0b1111:
			return "[r15]";
		default:
			break;
		}
		break;
	}

	return "[Unparsed]";
}

size_t
Operands(
	OperandType							aFirst,
	OperandType							aSecond,
	REXState							aREX,
	ModRMByte							aModRMByte,
	const std::vector<unsigned char>&	aImage,
	size_t								aNextByte,
	bool								aIsLEA,
	uint64_t*							aOutPointOfInterestData)
{
	size_t		end = aNextByte;
	int32_t		pointOfInterest		= 0;
	ReadWrite	pointOfInterestType = ReadWrite::READ;
	uint32_t	pointOfInterestSize = 1;

	uint32_t	extra;

	switch (aFirst)
	{
	case OperandType::IMM8:
	case OperandType::IMM16:
	case OperandType::IMM32:
	case OperandType::IMM64:
		printf("imm as first operand, code corrupted");
		return -1;
	case OperandType::REG8:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_8BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REG16:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_16BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REG32:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_32BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REG64:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_64BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REGMEM8:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_8BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 1;
		pointOfInterestType = ReadWrite::WRITE;
		break;
	case OperandType::REGMEM16:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_16BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 2;
		pointOfInterestType = ReadWrite::WRITE;
		break;
	case OperandType::REGMEM32:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_32BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 4;
		pointOfInterestType = ReadWrite::WRITE;
		break;
	case OperandType::REGMEM64:
		printf("%s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_64BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 8;
		pointOfInterestType = ReadWrite::WRITE;
		break;
	}

	switch (aSecond)
	{
	case OperandType::IMM8: {
		uint8_t imm;
		memcpy(&imm, aImage.data() + end, sizeof(imm));
		printf(", ");
		PRINTF_GREEN("$0x%02x", imm);
		end += sizeof(imm);
	}
	break;
	case OperandType::IMM16: {
		uint16_t imm;
		memcpy(&imm, aImage.data() + end, sizeof(imm));
		printf(", ");
		PRINTF_GREEN("$0x%04x", imm);
		end += sizeof(imm);
	}
	break;
	case OperandType::IMM32: {
		uint32_t imm;
		memcpy(&imm, aImage.data() + end, sizeof(imm));
		printf(", ");
		PRINTF_GREEN("$0x%08x", imm);
		end += sizeof(imm);
	}
	break;
	case OperandType::IMM64: {
		uint64_t imm;
		memcpy(&imm, aImage.data() + end, sizeof(imm));
		printf(", ");
		PRINTF_GREEN("$0x%016I64x", imm);
		end += sizeof(imm);
	}
	break;
	case OperandType::REG8:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_8BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REG16:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_16BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REG32:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_32BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REG64:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_64BIT, true, aImage, end, extra).c_str());
		break;
	case OperandType::REGMEM8:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_8BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 1;
		pointOfInterestType = ReadWrite::READ;
		break;
	case OperandType::REGMEM16:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_16BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 2;
		pointOfInterestType = ReadWrite::READ;
		break;
	case OperandType::REGMEM32:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_32BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 4;
		pointOfInterestType = ReadWrite::READ;
		break;
	case OperandType::REGMEM64:
		printf(", %s", RegMem(aModRMByte, aREX, RegisterSize::REGISTER_64BIT, false, aImage, end, extra, &pointOfInterest).c_str());
		end += extra;
		pointOfInterestSize = 8;
		pointOfInterestType = ReadWrite::READ;
		break;
	case OperandType::NONE:
		pointOfInterestType = ReadWrite::READ;
		break;
	default:
		break;
	}

	if (pointOfInterest != 0)
	{
		size_t location = pointOfInterest + end;

		SetColumn(globals::globalLocationColumn);

		if (aIsLEA)
		{
			PRINTF_GREEN("0x%08zx", location);
		}
		else
		{
			printf("0x%08zx", location);
		}
		
		switch (pointOfInterestType)
		{
		case ReadWrite::READ:
			if (aIsLEA)
				printf(" LOAD ADDR");
			else
				printf(" LOAD");

			{
				switch (pointOfInterestSize)
				{
				case 1: {
					uint8_t data;
					memcpy(&data, aImage.data() + location, sizeof(data));
					if (aIsLEA)
					{
						printf(" 0x%02x", data);
					}
					else
					{
						PRINTF_GREEN(" 0x%02x", data);
					}
					if (aOutPointOfInterestData)
						*aOutPointOfInterestData = data;
				}
				break;
				case 2: {
					uint16_t data;
					memcpy(&data, aImage.data() + location, sizeof(data));
					if (aIsLEA)
					{
						printf(" 0x%04x", data);
					}
					else
					{
						PRINTF_GREEN(" 0x%04x", data);
					}
					if (aOutPointOfInterestData)
						*aOutPointOfInterestData = data;
				}
				break;
				case 4: {
					uint32_t data;
					memcpy(&data, aImage.data() + location, sizeof(data));
					if (aIsLEA)
					{
						printf(" 0x%08x", data);
					}
					else
					{
						PRINTF_GREEN(" 0x%08x", data);
					}
					if (aOutPointOfInterestData)
						*aOutPointOfInterestData = data;
				}
				break;
				case 8: {
					uint64_t data;
					memcpy(&data, aImage.data() + location, sizeof(data));
					if (aIsLEA)
					{
						printf(" 0x%016I64x", data);
					}
					else
					{
						PRINTF_GREEN(" 0x%016I64x", data);
					}
					if (aOutPointOfInterestData)
						*aOutPointOfInterestData = data;
				}
				break;
				}
			}
			break;
		case ReadWrite::WRITE:
			printf(" STORE");
			break;
		}
	}

	return end;
}