#pragma once

#include "SceneObject.h"

#include <Serializable.h>
#include <Utils/Float3.h>
#include <Utils/Uint2.h>

namespace MG1
{
	class Torus : public SceneObject
	{
	public:
		explicit Torus() = default;
		Torus(const Torus& other) = default;

		Float3 position;
		Float3 rotation;
		Float3 scale;

		Uint2 samples;

		float largeRadius;
		float smallRadius;

		friend void to_json(nlohmann::ordered_json& j, const Torus& p);
		friend void from_json(const nlohmann::ordered_json& j, Torus& p);
	};
}
