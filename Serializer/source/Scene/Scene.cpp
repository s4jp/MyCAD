#include "pch.h"
#include "Scene.h"

#include <algorithm>

namespace MG1
{
	std::shared_ptr<Scene> Scene::s_instance = nullptr;

	Scene& MG1::Scene::Get()
	{
		if (!s_instance)
		{
			s_instance = std::shared_ptr<Scene>(new Scene());
		}

		return *s_instance;
	}
	
	void Scene::Clear()
	{
		SceneObject::s_firstFreeId = 1;
		SceneObject::s_freedIds = std::stack<uint32_t>();
		SceneObject::s_takenIds.clear();

		points.clear();
		tori.clear();

		bezierC0.clear();
		bezierC2.clear();
		interpolatedC2.clear();

		surfacesC0.clear();
		surfacesC2.clear();
	}
	
	bool Scene::IsValid() const
	{
		std::set<uint32_t> loadedPoints;

		// load all the points' ids
		for (auto& point : points)
		{
			loadedPoints.insert(point.GetId());
		}

		// validate if all control points exist
		for (auto& c : bezierC0)
		{
			if (std::any_of(c.controlPoints.begin(), c.controlPoints.end(), [&loadedPoints](PointRef ref) { return loadedPoints.find(ref.GetId()) == loadedPoints.end(); }))
			{
				return false;
			}
		}

		for (auto& c : bezierC2)
		{
			if (std::any_of(c.controlPoints.begin(), c.controlPoints.end(), [&loadedPoints](PointRef ref) { return loadedPoints.find(ref.GetId()) == loadedPoints.end(); }))
			{
				return false;
			}
		}

		for (auto& c : interpolatedC2)
		{
			if (std::any_of(c.controlPoints.begin(), c.controlPoints.end(), [&loadedPoints](PointRef ref) { return loadedPoints.find(ref.GetId()) == loadedPoints.end(); }))
			{
				return false;
			}
		}

		for (auto& surface : surfacesC0)
		{
			for (auto& patch : surface.patches)
			{
				if (std::any_of(patch.controlPoints.begin(), patch.controlPoints.end(), [&loadedPoints](PointRef ref) { return loadedPoints.find(ref.GetId()) == loadedPoints.end(); }))
				{
					return false;
				}
			}
		}

		for (auto& surface : surfacesC2)
		{
			for (auto& patch : surface.patches)
			{
				if (std::any_of(patch.controlPoints.begin(), patch.controlPoints.end(), [&loadedPoints](PointRef ref) { return loadedPoints.find(ref.GetId()) == loadedPoints.end(); }))
				{
					return false;
				}
			}
		}

		return true;
	}
}
