#pragma once

#include <vector>
#include <map>

#include <Models/Curves/BezierC0.h>
#include <Models/Curves/BezierC2.h>
#include <Models/Curves/InterpolatedC2.h>

#include <Models/Surfaces/BezierSurfaceC0.h>
#include <Models/Surfaces/BezierSurfaceC2.h>

#include <Models/Point.h>
#include <Models/Torus.h>

namespace MG1
{
	class Scene
	{
	public:
		static Scene& Get();

		void Clear();
		bool IsValid() const;

		std::vector<Point> points;
		std::vector<Torus> tori;

		std::vector<BezierC0> bezierC0;
		std::vector<BezierC2> bezierC2;
		std::vector<InterpolatedC2> interpolatedC2;

		std::vector<BezierSurfaceC0> surfacesC0;
		std::vector<BezierSurfaceC2> surfacesC2;

	protected:
		explicit Scene() = default;

		static std::shared_ptr<Scene> s_instance;
	};
}
