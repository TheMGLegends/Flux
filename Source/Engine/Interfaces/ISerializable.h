#pragma once

#include <nlohmann/json.hpp>

namespace nlohmann
{
	using flux_json = nlohmann::basic_json<nlohmann::ordered_map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;
}

namespace Flux
{
	class ISerializable
	{
	public:
		virtual ~ISerializable() = default;

		virtual void Serialize(nlohmann::flux_json& json) const = 0;
		virtual void Deserialize(const nlohmann::flux_json& json) = 0;
	};
}

