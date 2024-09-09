#include "pch.h"
#include "Torus.h"

#include <nlohmann/json.hpp>

namespace MG1
{
	void to_json(nlohmann::ordered_json& j, const Torus& p)
	{
		j = nlohmann::ordered_json {
			{ "objectType", "torus" },
			{ "id", p.m_id },
			{ "name", p.name },
			{ "smallRadius", p.smallRadius },
			{ "largeRadius", p.largeRadius },
		};

		j["position"] = p.position;
		j["rotation"] = p.rotation;
		j["scale"] = p.scale;
		j["samples"] = p.samples;
	}
	
	void from_json(const nlohmann::ordered_json& j, Torus& p)
	{
		if (j.contains("name"))
		{
			p.name = j["name"];
		}

		p.position = j["position"];
		p.rotation = j["rotation"];
		p.scale = j["scale"];

		p.samples = j["samples"];

		p.smallRadius = j["smallRadius"].get<float>();
		p.largeRadius = j["largeRadius"].get<float>();

		p.SetId(j["id"].get<uint32_t>());
	}
}
