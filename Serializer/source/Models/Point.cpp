#include <pch.h>

#include "Point.h"

#include <nlohmann/json.hpp>

namespace MG1
{
	Point::Point()
		: SceneObject(), position()
	{
	}

	void to_json(nlohmann::ordered_json& j, const Point& p)
	{
		j = {
			{ "id", p.m_id },
			{ "name", p.name.c_str() },
			{ "position", p.position }
		};
	}

	void from_json(const nlohmann::ordered_json& j, Point& p)
	{
		Float3 position = j["position"];
		
		if (j.contains("name"))
		{
			p.name = j["name"];
		}

		auto id = j["id"].get<uint32_t>();

		// TODO: check if the point was not a duplicate - SetId will return false
		p.SetId(id);
		p.position = position;
	}
}
