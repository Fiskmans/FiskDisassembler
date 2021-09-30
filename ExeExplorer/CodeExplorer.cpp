#include "CodeExplorer.h"
#include "Printers.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

struct BranchType
{
	std::string myName;
	size_t		myAddress;
	bool		myExpanded;
};
namespace globals {
	std::vector<BranchType> globalFunctions;
	std::vector<BranchType> globalBranches;
	const size_t			globalLocationColumn	 = 50;
	size_t					globalImageBase		   = 0;
} // globals

std::string
GenerateName(
	std::string									aName,
	std::string									aSuffix					= std::string())
{
	static size_t counter = 0; 
	return aName + std::to_string(++counter) + aSuffix;
}

std::string
GetOrGenerateFunctionName(
	size_t										aAddress,
	std::string									aSuffix					= std::string())
{
	static std::map<size_t, std::string> names;
	if (names.count(aAddress) == 0)
	{
		names[aAddress] = GenerateName("function_",aSuffix);

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
	size_t										aAddress,
	std::string									aSuffix					= std::string())
{
	static std::map<size_t, std::string> names;
	if (names.count(aAddress) == 0)
	{
		names[aAddress] = GenerateName("location_",aSuffix);
	}

	return names[aAddress];
}

void
AddFunction(
	BranchType									aFunction)
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
	size_t										aAddress,
	std::string									aSuffix					= std::string())
{
	for (BranchType& b : globals::globalFunctions)
		if (b.myAddress == aAddress)
			return;

	AddFunction({GetOrGenerateFunctionName(aAddress, aSuffix), aAddress, false});
}

void
AddBranch(
	size_t										aAddress,
	std::string									aSuffix					= std::string())
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
	const std::string&							aName)
{
	PRINTF_LIGHTBLUE("%s", aName.c_str());
}
void
PrintFunction(
	size_t										aAddress)
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
	size_t										aAddress)
{
	PrintLocation(GetOrGenerateLocationName(aAddress));
}

template <typename Type>
std::string
StringSignedHex(
	Type										aValue)
{
	if (aValue < 0)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(sizeof(Type) * 2)
			   << std::hex << -aValue;
		return "-" + stream.str();
	}
	else
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(sizeof(Type) * 2)
			   << std::hex << aValue;
		return "+" + stream.str();
	}
}

template <>
std::string
StringSignedHex<uint8_t>(
	uint8_t										aValue)
{
	if (aValue < 0)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(2)
			   << std::hex << -static_cast<int16_t>(aValue);
		return "-" + stream.str();
	}
	else
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(2)
			   << std::hex << static_cast<int16_t>(aValue);
		return "+" + stream.str();
	}
}

struct ModRMByte
{
	unsigned char	mod : 2;
	unsigned char	reg : 4;
	unsigned char	rm : 4;
};

struct SIBByte
{
	unsigned char	scale : 2;
	unsigned char	index : 4;
	unsigned char	base : 4;
};

struct REXState
{
	bool	w : 1;
	bool	r : 1;
	bool	x : 1;
	bool	b : 1;
};

typedef std::function<size_t(const std::vector<unsigned char>&, size_t, const std::vector<IMAGE_SECTION_HEADER>&, REXState, size_t)> Instruction;

ModRMByte
ParseModRM(
	unsigned char								aByte,
	REXState									aREX)
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
	unsigned char								aByte,
	REXState									aREX)
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
	ModRMByte									aModRM,
	SIBByte										aSIB)
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

enum class RegisterSize
{
	REGISTER_8BIT,
	REGISTER_16BIT,
	REGISTER_32BIT,
	REGISTER_64BIT
};

std::string
RegMem(
	ModRMByte									aModRM,
	REXState									aREX,
	RegisterSize								aRegisterSize,
	bool										aSelector,
	const std::vector<unsigned char>&			aImage,
	size_t										aNextByte,
	uint32_t&									aOutExtraConsumed,
	int32_t*									aOutMarkerAt			 = nullptr)
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
		uint8_t offset;
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
			uint8_t offsetWithSib;
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
	case 0b10:
		switch (aModRM.rm)
		{
		case 0b000:
			return "[rAX] + disp32";
		case 0b001:
			return "[rCX] + disp32";
		case 0b010:
			return "[rDX] + disp32";
		case 0b011:
			return "[rBX] + disp32";
		case 0b100:
			return "[SIB] + disp32";
		case 0b101:
			return "[how ??] + disp32";
		case 0b110:
			return "[rSI] + disp32";
		case 0b111:
			return "[rDI] + disp32";
		case 0b1000:
			return "[r8] + disp32";
		case 0b1001:
			return "[r9] + disp32";
		case 0b1010:
			return "[r10] + disp32";
		case 0b1011:
			return "[r11] + disp32";
		case 0b1100:
			return "[r12] + disp32";
		case 0b1101:
			return "[r13] + disp32";
		case 0b1110:
			return "[r14] + disp32";
		case 0b1111:
			return "[r15] + disp32";
		}
		break;
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

enum class OperandType
{
	IMM8,
	IMM16,
	IMM32,
	IMM64,
	REG8,
	REG16,
	REG32,
	REG64,
	REGMEM8,
	REGMEM16,
	REGMEM32,
	REGMEM64,
	NONE
};

enum class ReadWrite
{
	READ,
	WRITE
};

size_t
Operands(
	OperandType									aFirst,
	OperandType									aSecond,
	REXState									aREX,
	ModRMByte									aModRMByte,
	const std::vector<unsigned char>&			aImage,
	size_t										aNextByte,
	bool										aIsLEA					 = false,
	uint64_t*									aOutPointOfInterestData = nullptr)
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
		printf("0x%08zx", location);
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
					PRINTF_GREEN(" 0x%02x", data);
					if (aOutPointOfInterestData)
						*aOutPointOfInterestData = data;
				}
				break;
				case 2: {
					uint16_t data;
					memcpy(&data, aImage.data() + location, sizeof(data));
					PRINTF_GREEN(" 0x%04x", data);
					if (aOutPointOfInterestData)
						*aOutPointOfInterestData = data;
				}
				break;
				case 4: {
					uint32_t data;
					memcpy(&data, aImage.data() + location, sizeof(data));
					PRINTF_GREEN(" 0x%08x", data);
					if (aOutPointOfInterestData)
						*aOutPointOfInterestData = data;
				}
				break;
				case 8: {
					uint64_t data;
					memcpy(&data, aImage.data() + location, sizeof(data));
					PRINTF_GREEN(" 0x%016I64x", data);
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
	case 0x83:
		return Operands(OperandType::REGMEM64, OperandType::IMM8, aREX, byte, aImage, aExecutionPointer + 1);
	default:
		break;
	}
	return -1;
}

size_t
ADD(
	const std::vector<unsigned char>&				aImage,
	size_t											aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&		aSections,
	REXState										aREX,
	size_t											aInstructionBase)
{
	ModRMByte byte = ParseModRM(aImage[aExecutionPointer+1],aREX);

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
	}

	PRINTF_RED("unkown add type");
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
			PrintFunction(GenerateName("function_","_regaddr"));
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

	printf("unmapped cmp opcode %02x", aImage[aExecutionPointer]);
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

	printf("unmapped cmp opcode %02x", aImage[aExecutionPointer]);
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
ModRMExtension(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	std::map<unsigned char, std::map<unsigned char, Instruction>> ModRMExtesionTable;

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
	const std::vector<IMAGE_SECTION_HEADER>&	aSections)
{
	globals::globalImageBase = aImageBase;

	std::map<unsigned char, Instruction> legacyOpCodeTable;

	legacyOpCodeTable[0x0f] = SecondaryOPCodeTable;

	for (unsigned char	i   = 0x00; i <= 0x05; i++)
		legacyOpCodeTable[i] = ADD;

	for (unsigned char	i   = 0x30; i <= 0x35; i++)
		legacyOpCodeTable[i] = XOR;

	for (unsigned char	i   = 0x38; i <= 0x3d; i++)
		legacyOpCodeTable[i] = CMP;

	for (unsigned char	i   = 0x40; i <= 0x4F; i++)
		legacyOpCodeTable[i] = std::bind(REXPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));

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

	legacyOpCodeTable[0xc2] = RET;
	legacyOpCodeTable[0xc3] = RET;

	legacyOpCodeTable[0xc6] = ModRMExtension;
	legacyOpCodeTable[0xc7] = ModRMExtension;

	for (unsigned char	i   = 0xb0; i <= 0xbF; i++)
		legacyOpCodeTable[i] = MOV;

	legacyOpCodeTable[0xF6] = ModRMExtension;
	legacyOpCodeTable[0xF7] = ModRMExtension;

	legacyOpCodeTable[0xE8] = CALL_near_32_im;
	legacyOpCodeTable[0xE9] = JMPNear;
	legacyOpCodeTable[0xEB] = JMPNear;

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