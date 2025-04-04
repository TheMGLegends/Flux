#pragma once

#pragma warning (push)
#pragma warning (disable : 26495) // INFO: Disable warning for uninitialised variables
#include <nlohmann/json.hpp>
#pragma warning (pop)

namespace Flux
{
	class ISerializable
	{
	public:
		virtual ~ISerializable() = default;

		virtual void Serialize(nlohmann::ordered_json& json) const = 0;
		virtual void Deserialize(const nlohmann::ordered_json& json) = 0;
	};
}

