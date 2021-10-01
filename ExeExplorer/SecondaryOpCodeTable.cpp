#include "SecondaryOpCodeTable.h"

size_t
CPUID(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	size_t										aInstructionBase)
{
	printf("CPUID");
	return aExecutionPointer + 1;
}