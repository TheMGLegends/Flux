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

// INFO: Custom Assertion Macros
#ifdef FLUX_DEBUG
	#define FLUX_MINIMAL_ASSERT(expression) if (!(expression)) { __debugbreak(); }
	#define FLUX_CORE_ASSERT(expression, message) if (!(expression)) { FLUX_CORE_ERROR("Assertion Failed: {0}", message); __debugbreak(); }
	#define FLUX_ASSERT(expression, message) if (!(expression)) { FLUX_ERROR("Assertion Failed: {0}", message); __debugbreak(); }
#else
	#define FLUX_MINIMAL_ASSERT(expression) ((void)0)
	#define FLUX_CORE_ASSERT(expression, message) ((void)0)
	#define FLUX_ASSERT(expression, message) ((void)0)
#endif

// INFO: Bitmask Macro
#define BIT(x) (1 << (x))