#pragma once

#include <optional>
#include <string>

namespace Engine {
	class FileDialogs {
	public:
		static std::optional<std::string> openFile(const char* filter);
		static std::optional<std::string> saveFile(const char* filter);
	};
}
