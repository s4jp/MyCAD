#pragma once

#include "Point.h"

namespace MG1
{
	class PointRef
	{
	public:
		PointRef(uint32_t id);

		inline uint32_t GetId() const { return m_id; }

	protected:
		uint32_t m_id;
	};
}
