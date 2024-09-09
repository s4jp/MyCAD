#include "pch.h"
#include "Float3.h"

#include <nlohmann/json.hpp>

namespace MG1
{
	void to_json(nlohmann::ordered_json& j, const Float3& p)
	{
		j = {
			{ "x", p.x },
			{ "y", p.y },
			{ "z", p.z },
		};
	}
	
	void from_json(const nlohmann::ordered_json& j, Float3& p)
	{
		p.x = j["x"].get<float>();
		p.y = j["y"].get<float>();
		p.z = j["z"].get<float>();
	}
}
