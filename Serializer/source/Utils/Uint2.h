#pragma once

#include <Serializable.h>

namespace MG1
{
	class Uint2
	{
	public:
		uint32_t x, y;

		friend void to_json(nlohmann::ordered_json& j, const Uint2& p);
		friend void from_json(const nlohmann::ordered_json& j, Uint2& p);
	};
}
