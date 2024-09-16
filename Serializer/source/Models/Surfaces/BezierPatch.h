#pragma once

#include <Models/SceneObject.h>
#include <Models/PointRef.h>

#include <Utils/Uint2.h>

namespace MG1
{
	class BezierPatch : public SceneObject
	{
	public:
		explicit BezierPatch() = default;

        BezierPatch(const BezierPatch &other) = default;

        BezierPatch(BezierPatch &&other) = default;

		std::vector<PointRef> controlPoints;
		Uint2 samples;
	};
}
