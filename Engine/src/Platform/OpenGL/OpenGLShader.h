#pragma once

#include "Engine/Renderer/Shader.h"

#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Engine
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setInt(const std::string& name, int value) override;
		virtual void setIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void setFloat(const std::string& name, float value) override;
		virtual void setFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void setMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& getName() const override { return m_name; }

		void uploadUniformInt(const std::string& name, int value);
		void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void uploadUniformFloat(const std::string& name, float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string readFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> preprocess(const std::string& source);

		void compileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		void compileOrGetOpenGLBinaries();
		void createProgram();
		void reflect(GLenum stage, const std::vector<uint32_t>& shaderData);
	private:
		uint32_t m_rendererID;
		std::string m_filePath;
		std::string m_name;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_vulkanSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_openGLSPIRV;
		std::unordered_map<GLenum, std::string> m_openGLSourceCode;
	};
}
