#include "GlobalDefines.h"

namespace Flux::GlobalDefines
{
	const int FLUX_SUCCESS = 0;
	const int FLUX_FAILURE = 1;

	bool IS_FAILURE(int result)
	{
		return result != FLUX_SUCCESS;
	}
}
