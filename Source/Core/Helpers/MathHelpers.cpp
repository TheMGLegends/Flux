#include "MathHelpers.h"

#include <stdlib.h>

namespace Flux::MathHelpers
{
	float Clamp(float value, float min, float max)
	{
		if (value < min)
		{
			return min;
		}
		else if (value > max)
		{
			return max;
		}

		return value;
	}

	float Max(float x, float y, float z)
	{
		if (x > y && x > z)
		{
			return x;
		}
		else if (y > x && y > z)
		{
			return y;
		}
		else
		{
			return z;
		}
	}

	float RandomRange(float min, float max)
	{
		//INFO: Generate a random range between min and max(inclusive)
		float range = max - min;
		return min + static_cast<float>(rand()) / (RAND_MAX / range);
	}
}
