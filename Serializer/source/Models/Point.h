#pragma once

#include <map>
#include <memory>

#include "SceneObject.h"

#include <Serializable.h>
#include <Utils/Float3.h>

namespace MG1
{
	class Point : public SceneObject
	{
	public:
		explicit Point();
		Point(const Point& other) = default;

		virtual bool SetId(uint32_t id) override { return SceneObject::SetId(id); }

		Float3 position;

		friend void to_json(nlohmann::ordered_json& j, const Point& p);
		friend void from_json(const nlohmann::ordered_json& j, Point& p);

		friend class PointRef;
		friend class Scene;
	};
}
