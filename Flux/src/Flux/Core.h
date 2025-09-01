#pragma once

// INFO: Custom Success/Failure Macros
static constexpr int FLUX_SUCCESS = 0;
static constexpr int FLUX_FAILURE = 1;

static constexpr bool FLUX_FAILED(int result) { return result != FLUX_SUCCESS; }

// INFO: Custom Assertion Macros
#ifdef FLUX_ASSERTS_ENABLED
	#define FLUX_MINIMAL_ASSERT(expression) if (!(expression)) { __debugbreak(); }
	#define FLUX_CORE_ASSERT(expression, message) if (!(expression)) { FLUX_CORE_ERROR("Assertion Failed: {0}", message); __debugbreak(); }
	#define FLUX_ASSERT(expression, message) if (!(expression)) { FLUX_ERROR("Assertion Failed: {0}", message); __debugbreak(); }
#else
	#define FLUX_MINIMAL_ASSERT(expression) ((void)0)
	#define FLUX_CORE_ASSERT(expression, message) ((void)0)
	#define FLUX_ASSERT(expression, message) ((void)0)
#endif

// INFO: Verification Macros
#define FLUX_CORE_VERIFY(expression, message) if (!(expression)) { FLUX_CORE_ERROR("Verification Failed: {0}", message); }
#define FLUX_VERIFY(expression, message) if (!(expression)) { FLUX_ERROR("Verification Failed: {0}", message); }

// INFO: Bitmask Macro
#define BIT(x) (1 << (x))