#pragma once

#include <filesystem>

namespace Engine
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void onImGuiRender();

	private:
		std::filesystem::path m_currentDirectory;
	};
}
