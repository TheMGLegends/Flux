#pragma once

#include <cstdlib>

namespace Flux::GlobalDefines
{
	inline constexpr bool IS_FAILURE(int result) { return result != EXIT_SUCCESS;  }
}

