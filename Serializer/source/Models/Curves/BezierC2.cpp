#include "pch.h"
#include "BezierC2.h"

#include <iterator>
#include <nlohmann/json.hpp>

namespace MG1
{
    BezierC2::BezierC2(Bezier &&other)
            : Bezier(std::move(other))
    {   }

	void to_json(nlohmann::ordered_json& j, const BezierC2& p)
	{
		auto points = nlohmann::json::array();

		std::transform(
			p.controlPoints.begin(), p.controlPoints.end(),
			std::back_inserter(points),
			[](const PointRef& point) { return nlohmann::json{ { "id", point.GetId() } }; }
		);

		j = nlohmann::ordered_json {
			{ "objectType", "bezierC2" },
			{ "id", p.m_id },
			{ "name", p.name },
			{ "deBoorPoints", points }
		};
	}

	void from_json(const nlohmann::ordered_json& j, BezierC2& p)
	{
		if (j.contains("name"))
		{
			p.name = j["name"];
		}

		p.controlPoints.clear();

		for (auto& pointRef : j["deBoorPoints"])
		{
			auto id = pointRef["id"].get<uint32_t>();
			auto ref = PointRef(id);
			p.controlPoints.push_back(ref);
		}

		p.SetId(j["id"].get<uint32_t>());
	}
}
