#pragma once

// INFO: Windows Platform Macros
#ifdef FLUX_PLATFORM_WINDOWS
	#ifdef FLUX_BUILD_DLL
		#define FLUX_API __declspec(dllexport)
	#else
		#define FLUX_API __declspec(dllimport)
	#endif
#else
	#error FLUX_API macro not defined! Unknown platform!
#endif

// INFO: Custom Success/Failure Macros
constexpr int FLUX_SUCCESS = 0;
constexpr int FLUX_FAILURE = 1;

#define FLUX_FAILED(x) ((x) != FLUX_SUCCESS)

// INFO: Custom Assertion Macro
#ifdef FLUX_DEBUG
	#define FLUX_ASSERT(expression, message) assert((message, expression))
#else
	#define FLUX_ASSERT(expression, message) ((void)0)
#endif

// INFO: Bitmask Macro
#define BIT(x) (1 << (x))