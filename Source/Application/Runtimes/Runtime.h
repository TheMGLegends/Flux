#pragma once

namespace Flux
{
	class Runtime
	{
	public:
		Runtime() = default;
		virtual ~Runtime() = default;

		virtual void Release() = 0;
	};
}

