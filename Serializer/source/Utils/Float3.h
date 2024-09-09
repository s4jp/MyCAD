#pragma once

#include <Serializable.h>

namespace MG1
{
	class Float3
	{
	public:
		float x, y, z;

		friend void to_json(nlohmann::ordered_json& j, const Float3& p);
		friend void from_json(const nlohmann::ordered_json& j, Float3& p);
	};
}
