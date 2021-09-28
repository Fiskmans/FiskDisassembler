#include "CodeExplorer.h"
#include "Printers.h"

#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

struct BranchType
{
	std::string myName;
	size_t		myAddress;
};

std::string
GetOrGenerateFunctionName(size_t				aAddress)
{
	static std::map<size_t, std::string> names;
	if (names.count(aAddress) == 0)
	{
		names[aAddress] = "function_" + std::to_string(names.size());
	}

	return names[aAddress];
}

std::vector<BranchType>																													globalFunctions;

void
AddFunction(
	std::string									aName,
	size_t										aAddress)
{
	globalFunctions.push_back({aName, aAddress});
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

typedef std::function<size_t(const std::vector<unsigned char>&, size_t, const std::vector<IMAGE_SECTION_HEADER>&, REXState, size_t)>	Instruction;

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

std::string
RegMem64(
	ModRMByte									aModRM,
	REXState									aREX,
	bool										aSelector,
	const std::vector<unsigned char>&			aImage,
	size_t										aNextByte,
	uint32_t&									aOutExtraConsumed,
	int32_t*									aOutMarkerAt = nullptr)
{
	aOutExtraConsumed = 0;

	switch (aModRM.mod)
	{
	case 0b11:
		switch (aSelector ? aModRM.reg : aModRM.rm)
		{
		case 0b000:
			return "[rAX]";
		case 0b001:
			return "[rCX]";
		case 0b010:
			return "[rDX]";
		case 0b011:
			return "[rBX]";
		case 0b100:
			return "[rSP]";
		case 0b101:
			return "[rBP]";
		case 0b110:
			return "[rSI]";
		case 0b111:
			return "[rDI]";
		default:
			break;
		}
		break;
	case 0b01:
		switch (aSelector ? aModRM.reg : aModRM.rm)
		{
		case 0b000:
			return "[rAX] + disp8";
		case 0b001:
			return "[rCX] + disp8";
		case 0b010:
			return "[rDX] + disp8";
		case 0b011:
			return "[rBX] + disp8";
		case 0b100:
			return "[SIB] + disp8";
		case 0b101:
			return "[how ??] + disp8";
		case 0b110:
			return "[rSI] + disp8";
		case 0b111:
			return "[rDI] + disp8";
		default:
			break;
		}
		break;
	case 0b10:
		switch (aSelector ? aModRM.reg : aModRM.rm)
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
		default:
			break;
		}
		break;
	case 0b00:
		switch (aSelector ? aModRM.reg : aModRM.rm)
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
			if (offset < 0)
			{
				std::stringstream stream;
				stream << std::setfill('0') << std::setw(sizeof(uint32_t) * 2)
					   << std::hex << -offset;
				out += "- " + stream.str();
			}
			else
			{
				std::stringstream stream;
				stream << std::setfill('0') << std::setw(sizeof(uint32_t) * 2)
					   << std::hex << offset;
				out += "+ " + stream.str();
			}
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
		default:
			break;
		}
		break;
	}

	return "[Unparsed]";
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

	int32_t offset;
	memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));

	if (offset > 0)
	{
		printf("+%04x ", offset);
	}
	else
	{
		printf("-%04x ", -offset);
	}
	printf("(0x%08zx)", aExecutionPointer + 5 + offset);
	return aExecutionPointer + 5 + offset;
}

size_t
SUB_RM_IM8(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("SUB %s, ", RegMem64(byte, aREX, false, aImage, aExecutionPointer + 1, extra).c_str());

	printf("0x%02x", aImage[aExecutionPointer + 1 + extra]);
	return aExecutionPointer + 2 + extra;
}

size_t
ADD_RM_IM8(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	uint32_t	extra	 = 0;
	printf("ADD %s, ", RegMem64(byte, aREX, false, aImage, aExecutionPointer + 1, extra).c_str());

	printf("0x%02x", aImage[aExecutionPointer + 1 + extra]);
	return aExecutionPointer + 2 + extra;
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

	if (offset > 0)
	{
		printf("+%04x ", offset);
	}
	else
	{
		printf("-%04x ", -offset);
	}
	printf("\t\t0x%08zx  %s", target, name.c_str());

	AddFunction(name, target);

	return nextInstruction;
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
	case 0xb0:
		break;
	case 0xb1:
		break;
	case 0xb2:
		break;
	case 0xb3:
		break;
	case 0xb4:
		break;
	case 0xb5:
		break;
	case 0xb6:
		break;
	case 0xb7:
		break;
	case 0xb8: {
		if (aREX.w)
		{
			uint64_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf("[rAX], 0x%016I64x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
		else
		{
			uint32_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf("[rAX], 0x%08x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
	}
	break;
	case 0xb9: {
		if (aREX.w)
		{
			uint64_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf("[rCX], 0x%016I64x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
		else
		{
			uint32_t data;
			memcpy(&data, aImage.data() + aExecutionPointer + 1, sizeof(data));
			printf("[rCX], 0x%08x", data);
			return aExecutionPointer + 1 + sizeof(data);
		}
	}
	break;
	case 0xba:
		break;
	case 0xbb:
		break;
	case 0xbc:
		break;
	case 0xbd:
		break;
	case 0xbe:
		break;
	case 0xbf:
		break;
	default:
		break;
	}

	PrintAround(aImage, aExecutionPointer + 1);
	return -1;
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

	switch (aImage[aExecutionPointer])
	{
	case 0x39: {
		ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer + 1], aREX);
		uint32_t	extra1	 = 0;
		uint32_t	extra2	 = 0;

		int32_t		offset	= 0;

		printf("%s", RegMem64(byte, aREX, false, aImage, aExecutionPointer + 2, extra1, &offset).c_str());
		printf(", %s", RegMem64(byte, aREX, true, aImage, aExecutionPointer + 2, extra2).c_str());

		if (offset != 0)
		{
			size_t loc = aExecutionPointer + 2 + extra1 + offset;
			printf("\t0x%08zx", loc);
			if (aREX.w)
			{
				uint64_t toCmp;
				memcpy(&toCmp,aImage.data() + loc,sizeof(toCmp));
				printf(": %016I64x",toCmp);
			}

		}

		return aExecutionPointer + 2 + extra1;
	}
	break;
	}

	printf("unmapped cmp opcode %02x", aImage[aExecutionPointer]);
	PrintAround(aImage, aExecutionPointer + 1);
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

	ModRMExtesionTable[0x83][0x0]	= ADD_RM_IM8;
	ModRMExtesionTable[0x83][0x5]	= SUB_RM_IM8;

	ModRMExtesionTable[0xc7][0x0]	= MOV;

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

void
ExploreCode(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionStart,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections)
{
	std::map<unsigned char, Instruction> legacyOpCodeTable;

	for (unsigned char	i   = 0x38; i <= 0x3d; i++)
		legacyOpCodeTable[i] = CMP;

	for (unsigned char	i   = 0x40; i <= 0x4F; i++)
		legacyOpCodeTable[i] = std::bind(REXPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::ref(legacyOpCodeTable));

	legacyOpCodeTable[0x80] = ModRMExtension;
	legacyOpCodeTable[0x81] = ModRMExtension;
	legacyOpCodeTable[0x82] = ModRMExtension;
	legacyOpCodeTable[0x83] = ModRMExtension;

	for (unsigned char	i   = 0x88; i <= 0x8c; i++)
		legacyOpCodeTable[i] = MOV;

	legacyOpCodeTable[0xc2] = RET;
	legacyOpCodeTable[0xc3] = RET;

	legacyOpCodeTable[0xc6] = ModRMExtension;
	legacyOpCodeTable[0xc7] = ModRMExtension;

	for (unsigned char	i   = 0xb0; i <= 0xbF; i++)
		legacyOpCodeTable[i] = MOV;

	legacyOpCodeTable[0xE8] = CALL_near_32_im;
	legacyOpCodeTable[0xE9] = JMPNear;

	REXState rex;
	rex.w	= false;
	rex.r	= false;
	rex.x	= false;
	rex.b	= false;

	AddFunction("Main", aExecutionStart);
	for (size_t			i	= 0; i < globalFunctions.size(); i++)
	{
		printf("%08zx [%s]", globalFunctions[i].myAddress, globalFunctions[i].myName.c_str());
		size_t executionPointer = globalFunctions[i].myAddress;

		while (executionPointer != -1)
		{
			printf("\n  0x%08zx ", executionPointer);
			if (legacyOpCodeTable.count(aImage[executionPointer]))
			{
				executionPointer = legacyOpCodeTable[aImage[executionPointer]](aImage, executionPointer, aSections, rex, executionPointer);
			}
			else
			{
				PRINTF_RED("Unkown instruction: %02x\n", aImage[executionPointer]);
				PrintAround(aImage, executionPointer, aSections);
				break;
			}
		}
		printf("\n\n");
	}
}