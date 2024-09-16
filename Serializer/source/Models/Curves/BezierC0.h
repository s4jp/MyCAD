#pragma once

#include "Bezier.h"

namespace MG1
{
	class BezierC0 : public Bezier
	{
	public:
        explicit BezierC0() = default;

        explicit BezierC0(Bezier &&other);

		friend void to_json(nlohmann::ordered_json& j, const BezierC0& p);
		friend void from_json(const nlohmann::ordered_json& j, BezierC0& p);
	};
}
