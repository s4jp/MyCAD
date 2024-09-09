#pragma once

#include "Bezier.h"

namespace MG1
{
	class BezierC2 : public Bezier
	{
	public:
        explicit BezierC2() = default;

        explicit BezierC2(Bezier &&other);

		friend void to_json(nlohmann::ordered_json& j, const BezierC2& p);
		friend void from_json(const nlohmann::ordered_json& j, BezierC2& p);
	};
}
