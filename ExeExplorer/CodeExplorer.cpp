#include "CodeExplorer.h"
#include "Printers.h"

#include <functional>
#include <map>
#include <string>

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

typedef std::function<size_t(const std::vector<unsigned char>&, size_t, const std::vector<IMAGE_SECTION_HEADER>&, REXState)> Instruction;

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
	case  0b000:
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
	bool&										aConsumedExtraByte)
{
	aConsumedExtraByte = false;

	if (aModRM.mod == 0b11)
	{
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
			aConsumedExtraByte	= true;

			SIBByte sib			= ParseSIB(aImage[aNextByte], aREX);

			return RegMemSIB(aModRM, sib);
		}
		case 0b101:
			return "[disp32]";
		case 0b110:
			return "[rSI]";
		case 0b111:
			return "[rDI]";
		default:
			break;
		}
	}
	else
	{
		switch (aModRM.mod)
		{
		case 0b01:
			switch (aModRM.rm)
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
			default:
				break;
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
			case 0b100:
				return "[SIB]";
			case 0b101:
				return "[how ??]";
			case 0b110:
				return "[rSI]";
			case 0b111:
				return "[rDI]";
			default:
				break;
			}
			break;
		}
	}

	return "[Unparsed]";
}

size_t
JMPNear(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX)
{
	printf("JMP near ");

	int32_t offset;
	memcpy(&offset, aImage.data() + aExecutionPointer + 1, sizeof(offset));

	if (offset > 0)
	{
		printf("+%04x\n", offset);
	}
	else
	{
		printf("-%04x\n", offset);
	}
	return aExecutionPointer + 5 + offset;
}

size_t
SUB_RM_IM8(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX)
{
	ModRMByte	byte	  = ParseModRM(aImage[aExecutionPointer], aREX);
	bool		extra	 = false;
	printf("SUB %s,", RegMem64(byte, aREX, false, aImage, aExecutionPointer + 1, extra).c_str());

	printf("%02x\n", aImage[aExecutionPointer + (extra ? 2 : 1)]);
	return aExecutionPointer + (extra ? 3 : 2);
}

size_t
MOV(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX)
{
	printf("MOV \n");

	PrintAround(aImage,aExecutionPointer + 1);
	return -1;
}

size_t
ModRMExtension(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX)
{
	std::map<unsigned char, std::map<unsigned char, Instruction>> ModRMExtesionTable;

	ModRMExtesionTable[0x83][0x5] = SUB_RM_IM8;

	ModRMExtesionTable[0xc7][0x0] = MOV;





	if (ModRMExtesionTable.count(aImage[aExecutionPointer]) != 0)
	{
		ModRMByte byte = ParseModRM(aImage[aExecutionPointer + 1], aREX);

		if (ModRMExtesionTable[aImage[aExecutionPointer]].count(byte.reg) != 0)
		{
			return ModRMExtesionTable[aImage[aExecutionPointer]][byte.reg](aImage, aExecutionPointer + 1, aSections, aREX);
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

		return (it->second)(aImage, aExecutionPointer + 1, aSections, rex);
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

	legacyOpCodeTable[0x40] = std::bind(REXPrefix, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::ref(legacyOpCodeTable));
	for (size_t i = 0x41; i <= 0x4F; i++)
		legacyOpCodeTable[static_cast<unsigned char>(i)] = legacyOpCodeTable[0x40];

	legacyOpCodeTable[0x80]			= ModRMExtension;
	legacyOpCodeTable[0x81]			= ModRMExtension;
	legacyOpCodeTable[0x82]			= ModRMExtension;
	legacyOpCodeTable[0x83]			= ModRMExtension;

	legacyOpCodeTable[0xc6]			= ModRMExtension;
	legacyOpCodeTable[0xc7]			= ModRMExtension;

	legacyOpCodeTable[0xE9]			= JMPNear;

	size_t		executionPointer	   = aExecutionStart;
	REXState	rex;
	rex.w	= false;
	rex.r	= false;
	rex.x	= false;
	rex.b	= false;

	while (executionPointer != -1)
	{
		if (legacyOpCodeTable.count(aImage[executionPointer]))
		{
			executionPointer = legacyOpCodeTable[aImage[executionPointer]](aImage, executionPointer, aSections, rex);
		}
		else
		{
			PRINTF_RED("Unkown instruction: %02x\n", aImage[executionPointer]);
			PrintAround(aImage, executionPointer, aSections);
			break;
		}
	}
}