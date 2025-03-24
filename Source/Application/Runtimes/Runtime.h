#pragma once

namespace Flux
{
	class Runtime
	{
	public:
		Runtime() = default;
		virtual ~Runtime() {}

		virtual void Release() = 0;
	};
}

