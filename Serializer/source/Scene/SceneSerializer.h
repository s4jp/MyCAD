#pragma once

#include <filesystem>

#include "Scene.h"

namespace MG1
{
	class SceneSerializer
	{
	public:
		Scene& LoadScene(std::filesystem::path path);
		void SaveScene(std::filesystem::path path);

	private:
		nlohmann::json LoadAndValidate(std::filesystem::path);
	};
}
