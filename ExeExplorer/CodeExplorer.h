#pragma once

#include <vector>
#define NOMINMAX
#include <windows.h>

void ExploreCode(
			const std::vector<unsigned char>&			aImage,
			size_t										aExecutionStart,
			const std::vector<IMAGE_SECTION_HEADER>&	aSections);