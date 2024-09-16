#pragma once

#include <vector>

#include <Serializable.h>

#include <Models/SceneObject.h>
#include <Models/PointRef.h>

namespace MG1
{
	class Bezier : public SceneObject
	{
	public:
		explicit Bezier() = default;

        Bezier(const Bezier &other) = default;

        Bezier(Bezier &&other) = default;

		std::vector<PointRef> controlPoints;

		friend void to_json(nlohmann::ordered_json& j, const Bezier& p);
		friend void from_json(const nlohmann::ordered_json& j, Bezier& p);
	};
}
