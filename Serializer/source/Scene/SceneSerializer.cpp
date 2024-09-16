#include "pch.h"
#include "SceneSerializer.h"

#include "SerializerException.h"

#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/utils/nlohmann_json_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

#include <Resources/Schema.h>

namespace MG1
{
	Scene& SceneSerializer::LoadScene(std::filesystem::path path)
	{
		auto document = LoadAndValidate(path);

		auto& resultScene = Scene::Get();
		resultScene.Clear();

		std::set<uint32_t> loadedPoints;

		for (auto& point : document["points"])
		{
			Point p = point;
			resultScene.points.push_back(p);

			loadedPoints.insert(p.GetId());
		}

		for (auto& element : document["geometry"])
		{
			auto elementType = element["objectType"].get<std::string>();
			
			// TODO: could be done smarter than this massive conditional block
			if (elementType == "torus")
			{
				Torus t = element;
				resultScene.tori.push_back(t);
			}
			else if (elementType == "bezierC0")
			{
				BezierC0 b = element;
				resultScene.bezierC0.push_back(b);
			}
			else if (elementType == "bezierC2")
			{
				BezierC2 b = element;
				resultScene.bezierC2.push_back(b);
			}
			else if (elementType == "interpolatedC2")
			{
				InterpolatedC2 b = element;
				resultScene.interpolatedC2.push_back(b);
			}
			else if (elementType == "bezierSurfaceC0")
			{
				BezierSurfaceC0 s = element;
				resultScene.surfacesC0.push_back(s);
			}
			else if (elementType == "bezierSurfaceC2")
			{
				BezierSurfaceC2 s = element;
				resultScene.surfacesC2.push_back(s);
			}
		}

		if (!resultScene.IsValid())
		{
			throw SerializerException("File corrupted - Control points are not valid!");
		}

		return resultScene;
	}
	
	void SceneSerializer::SaveScene(std::filesystem::path path)
	{
		auto& scene = Scene::Get();

		nlohmann::ordered_json dumpDocument;

		auto points = nlohmann::ordered_json::array();
		
		for (auto& point : scene.points)
		{
			points.push_back(point);
		}

		auto geometry = nlohmann::ordered_json::array();

		for (auto& obj : scene.tori)
		{
			geometry.push_back(obj);
		}

		for (auto& obj : scene.bezierC0)
		{
			geometry.push_back(obj);
		}

		for (auto& obj : scene.bezierC2)
		{
			geometry.push_back(obj);
		}

		for (auto& obj : scene.interpolatedC2)
		{
			geometry.push_back(obj);
		}

		for (auto& obj : scene.surfacesC0)
		{
			geometry.push_back(obj);
		}

		for (auto& obj : scene.surfacesC2)
		{
			geometry.push_back(obj);
		}

		dumpDocument["geometry"] = geometry;
		dumpDocument["points"] = points;

		auto jsonString = dumpDocument.dump(1, '\t');

		std::ofstream out(path);
		out << jsonString;
		out.close();
	}
	
	nlohmann::json SceneSerializer::LoadAndValidate(std::filesystem::path path)
	{
		nlohmann::json document;
		
		if (!valijson::utils::loadDocument(path.string(), document))
		{
			throw SerializerException("Loading the json file failed");
		}

		// TODO: think about how the schema file is included - pasting it with every update seems unnecessary
		nlohmann::json schemaJson = nlohmann::json::parse(g_schema);

		valijson::Schema schema;
		valijson::SchemaParser parser;
		valijson::adapters::NlohmannJsonAdapter schemaAdapter(schemaJson);
		
		parser.populateSchema(schemaAdapter, schema);

		valijson::Validator validator;
		valijson::adapters::NlohmannJsonAdapter sceneAdapter(document);
		valijson::ValidationResults validationResults;
		
		if (!validator.validate(schema, sceneAdapter, &validationResults))
		{
			throw SerializerException("Scene file is corrupted.");
		}

		return document;
	}
}
