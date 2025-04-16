#pragma once

#include <cstdlib>

namespace Flux::GlobalDefines
{
	extern const int FLUX_SUCCESS; 
	extern const int FLUX_FAILURE;

	/// @param result An integer result code of 0 is success, non-zero is failure (Typically 1)
	bool FLUX_FAIL(int result);
}

