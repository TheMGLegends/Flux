#pragma once

#include <nlohmann/json.hpp>

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

