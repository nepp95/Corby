#pragma once

#include <string>

namespace Engine {
	class Shader {
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Shader* create(const std::string& path);
		static Shader* create(const std::string& vertexSrc, const std::string& fragmentSrc);

	private:
		uint32_t m_rendererID;
	};
}