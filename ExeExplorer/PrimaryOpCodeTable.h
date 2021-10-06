#pragma once
#include "CodeCommon.h"

//0x00
size_t
ADD(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
OR(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

//0x10

//0x20
size_t
AND(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
SUB(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

//0x30
size_t
CMP(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
XOR(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

//0x60
size_t
IMUL(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

//0x70
size_t
JCC(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

//0x80
size_t
XCHG(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
TEST(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
LEA(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

//0x90
size_t
CBW(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

//0xA0
size_t
MOVS(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

//0xC0
size_t
RET(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

size_t
INT3(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

//0xE0
size_t
CALL(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

size_t
JMP(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

//0xF0
size_t
CLC(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
MOVSXD(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);


size_t
STOS(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

size_t
MOV(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool aIs16Bit,
	size_t										aInstructionBase);

size_t
PUSH(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
POP(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);

size_t
INT_Instruction(
	const std::vector<unsigned char>&			aImage,
	size_t										aExecutionPointer,
	const std::vector<IMAGE_SECTION_HEADER>&	aSections,
	REXState									aREX,
	bool										aIs16Bit,
	size_t										aInstructionBase);
