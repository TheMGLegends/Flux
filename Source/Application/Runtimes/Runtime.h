#pragma once

namespace Flux
{
	class Runtime
	{
	public:
		Runtime() = default;
		virtual ~Runtime() = 0 {}

		virtual void Release() = 0;
	};
}

