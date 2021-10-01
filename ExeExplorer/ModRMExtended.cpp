#include "ModRMExtended.h"

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