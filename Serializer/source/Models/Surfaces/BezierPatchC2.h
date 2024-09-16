#pragma once

#include "BezierPatch.h"

namespace MG1
{
	class BezierPatchC2 : public BezierPatch
	{
	public:
        explicit BezierPatchC2() = default;

        explicit BezierPatchC2(BezierPatch &&other);

		friend void to_json(nlohmann::ordered_json& j, const BezierPatchC2& p);
		friend void from_json(const nlohmann::ordered_json& j, BezierPatchC2& p);
	};
}
