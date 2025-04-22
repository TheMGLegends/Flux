#include "MathHelpers.h"

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
}
