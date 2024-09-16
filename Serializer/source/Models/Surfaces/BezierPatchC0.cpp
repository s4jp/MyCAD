#include "pch.h"
#include "BezierPatchC0.h"

#include <nlohmann/json.hpp>
#include <algorithm>

namespace MG1
{
    BezierPatchC0::BezierPatchC0(BezierPatch &&other)
            : BezierPatch(std::move(other))
    {   }

	void to_json(nlohmann::ordered_json& j, const BezierPatchC0& p)
	{
		auto points = nlohmann::json::array();

		std::transform(
			p.controlPoints.begin(), p.controlPoints.end(),
			std::back_inserter(points),
			[](const PointRef& point) { return nlohmann::json{ { "id", point.GetId() } }; }
		);

		j = nlohmann::ordered_json {
			{ "objectType", "bezierPatchC0" },
			{ "id", p.m_id },
			{ "name", p.name },
			{ "controlPoints", points },
		};

		j["samples"] = p.samples;
	}

	void from_json(const nlohmann::ordered_json& j, BezierPatchC0& p)
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
