#pragma once

#include <Models/SceneObject.h>
#include <Models/Surfaces/BezierPatchC0.h>
#include <Utils/Uint2.h>

namespace MG1
{
	class BezierSurfaceC0 : public SceneObject
	{
	public:
		friend void to_json(nlohmann::ordered_json& j, const BezierSurfaceC0& p);
		friend void from_json(const nlohmann::ordered_json& j, BezierSurfaceC0& p);

		bool uWrapped, vWrapped;
		Uint2 size;
		std::vector<BezierPatchC0> patches;
	};
}
