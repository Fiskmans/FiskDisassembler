#pragma once

#define NOMINMAX
#include <windows.h>

#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

struct BranchType
{
	std::string myName;
	size_t		myAddress;
	bool		myExpanded;
};
namespace globals {
	extern std::vector<BranchType>	globalFunctions;
	extern std::vector<BranchType>	globalBranches;
	extern const size_t				globalLocationColumn;
	extern size_t					globalImageBase;
} // globals

std::string
						GenerateName(
							std::string							aName,
							std::string							aSuffix					= std::string());

std::string
						GetOrGenerateFunctionName(
							size_t								aAddress,
							std::string							aSuffix					= std::string());

std::string
						GetOrGenerateLocationName(
							size_t								aAddress,
							std::string							aSuffix					= std::string());

void
						AddFunction(
							BranchType							aFunction);

void
						AddFunction(
							size_t								aAddress,
							std::string							aSuffix					= std::string());

void
						AddBranch(
							size_t								aAddress,
							std::string							aSuffix					= std::string());

void
						PrintFunction(
							const std::string&					aName);

void
						PrintFunction(
							size_t								aAddress);

void
						PrintLocation(
							const std::string&					aName);

void
						PrintLocation(
							size_t								aAddress);

template <typename Type>
std::string
StringSignedHex(
	Type														aValue)
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
std::string inline StringSignedHex<int8_t>(
	int8_t														aValue)
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

typedef std::function<	size_t(const std::vector<unsigned char>&, size_t, const std::vector<IMAGE_SECTION_HEADER>&, REXState, size_t)> Instruction;

ModRMByte
						ParseModRM(
							unsigned char						aByte,
							REXState							aREX);

SIBByte
						ParseSIB(
							unsigned char						aByte,
							REXState							aREX);

std::string
						RegMemSIB(
							ModRMByte							aModRM,
							SIBByte								aSIB);

enum class RegisterSize
{
	REGISTER_8BIT,
	REGISTER_16BIT,
	REGISTER_32BIT,
	REGISTER_64BIT
};

std::string
						RegMem(
							ModRMByte							aModRM,
							REXState							aREX,
							RegisterSize						aRegisterSize,
							bool								aSelector,
							const std::vector<unsigned char>&	aImage,
							size_t								aNextByte,
							uint32_t&							aOutExtraConsumed,
							int32_t*							aOutMarkerAt			 = nullptr);

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
							OperandType							aFirst,
							OperandType							aSecond,
							REXState							aREX,
							ModRMByte							aModRMByte,
							const std::vector<unsigned char>&	aImage,
							size_t								aNextByte,
							bool								aIsLEA					 = false,
							uint64_t*							aOutPointOfInterestData = nullptr);