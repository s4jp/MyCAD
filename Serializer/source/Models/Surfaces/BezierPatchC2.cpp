#include "pch.h"
#include "BezierPatchC2.h"

#include <nlohmann/json.hpp>
#include <algorithm>

namespace MG1
{
    BezierPatchC2::BezierPatchC2(BezierPatch &&other)
            : BezierPatch(std::move(other))
    {   }

	void to_json(nlohmann::ordered_json& j, const BezierPatchC2& p)
	{
		auto points = nlohmann::ordered_json::array();

		std::transform(
			p.controlPoints.begin(), p.controlPoints.end(),
			std::back_inserter(points),
			[](const PointRef& point) { return nlohmann::ordered_json { { "id", point.GetId() } }; }
		);

		j = nlohmann::ordered_json {
			{ "objectType", "bezierPatchC2" },
			{ "id", p.m_id },
			{ "name", p.name },
			{ "controlPoints", points }
		};

		j["samples"] = p.samples;
	}
	
	void from_json(const nlohmann::ordered_json& j, BezierPatchC2& p)
	{
		if (j.contains("name"))
		{
			p.name = j["name"];
		}

		p.controlPoints.clear();

		for (auto& pointRef : j["controlPoints"])
		{
			auto id = pointRef["id"].get<uint32_t>();
			auto ref = PointRef(id);
			p.controlPoints.push_back(ref);
		}

		p.samples = j["samples"];
		p.SetId(j["id"].get<uint32_t>());
	}
}
