#pragma once

#include <Models/SceneObject.h>
#include <Models/Surfaces/BezierPatchC2.h>
#include <Utils/Uint2.h>

namespace MG1
{
	class BezierSurfaceC2 : public SceneObject
	{
	public:
		friend void to_json(nlohmann::ordered_json& j, const BezierSurfaceC2& p);
		friend void from_json(const nlohmann::ordered_json& j, BezierSurfaceC2& p);

		bool uWrapped, vWrapped;
		Uint2 size;
		std::vector<BezierPatchC2> patches;
	};
}
