#pragma once

#include <cstdlib>

namespace Flux::GlobalDefines
{
	inline constexpr int FLUX_SUCCESS = 0; 
	inline constexpr int FLUX_FAILURE = 1;

	inline constexpr bool IS_FAILURE(int result) { return result != FLUX_SUCCESS;  }
}

