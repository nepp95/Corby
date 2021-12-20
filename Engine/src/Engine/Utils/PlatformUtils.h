#pragma once

#include <optional>
#include <string>

namespace Engine
{
	class FileDialogs
	{
	public:
		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	};
}
