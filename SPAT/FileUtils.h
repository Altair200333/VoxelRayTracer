#pragma once
#include <nfd/nfd.h>
#include <optional>
#include <string>
#include <fstream>

namespace FileUtils
{
	inline std::optional<std::string> openFileDialog()
	{
		nfdchar_t* outPath = nullptr;
		const nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

		if (result == NFD_OKAY)
		{
			std::string path = std::string(outPath);
			free(outPath);
			return { path };
		}

		return {};
	}

	inline std::string fileContents(const std::string& path)
	{
		std::ifstream file(path);
		if (!file)
			return "";
		return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	}
	inline std::string fileExtension(const std::string& path)
	{
		return path.substr(path.find_last_of('.') + 1);
	}
}
