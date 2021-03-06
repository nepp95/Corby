#include "engpch.h"
#include "OpenGLShader.h"

#include "Engine/Core/Timer.h"

#include <fstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Engine
{
	namespace Utils
	{
		// Temporary shader fix. AMD GPU's crash with spirv
		static const bool IsAmdGpu()
		{
			const char* vendor = (char*) glGetString(GL_VENDOR);
			return strstr(vendor, "ATI") != nullptr;
		}

		static bool VerifyProgramLink(GLenum& program)
		{
			int isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int*) &isLinked);
			if (isLinked == GL_FALSE)
			{
				int maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<char> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

				glDeleteProgram(program);

				ENG_CORE_ERROR("{0}", infoLog.data());
				ENG_CORE_ASSERT(false, "[OpenGL] Shader link failure!");
				return false;
			}
			return true;
		}
		// ^^^Temporary shader fix. AMD GPU's crash with spirv

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			ENG_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:
					return shaderc_glsl_vertex_shader;
				case GL_FRAGMENT_SHADER:
					return shaderc_glsl_fragment_shader;
			}

			ENG_CORE_ASSERT(false);
			return (shaderc_shader_kind) 0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:
					return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER:
					return "GL_FRAGMENT_SHADER";
			}

			ENG_CORE_ASSERT(false);
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: Do not hardcode this :(
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:
					return ".cached_opengl.vert";
				case GL_FRAGMENT_SHADER:
					return ".cached_opengl.frag";
			}

			ENG_CORE_ASSERT(false);
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:
					return ".cached_vulkan.vert";
				case GL_FRAGMENT_SHADER:
					return ".cached_vulkan.frag";
			}

			ENG_CORE_ASSERT(false);
			return "";
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_filePath(filepath)
	{
		ENG_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		auto shaderSources = Preprocess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			if (Utils::IsAmdGpu())
			{
				CreateProgramForAmdGpu();
			} else
			{
				CompileOrGetOpenGLBinaries();
				CreateProgram();
			}

			ENG_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_name(name)
	{
		ENG_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		if (Utils::IsAmdGpu())
		{
			CreateProgramForAmdGpu();
		} else
		{
			CompileOrGetOpenGLBinaries();
			CreateProgram();
		}
	}

	OpenGLShader::~OpenGLShader()
	{
		ENG_PROFILE_FUNCTION();

		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::Bind() const
	{
		ENG_PROFILE_FUNCTION();

		glUseProgram(m_rendererID);
	}

	void OpenGLShader::Unbind() const
	{
		ENG_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		ENG_PROFILE_FUNCTION();

		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		std::string result;

		if (in)
		{
			// Find out the size of the file and resize the string accordingly
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();

			if (size != -1)
			{
				result.resize(in.tellg());

				// Put the file contents in the string
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
			} else
			{
				ENG_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		} else
		{
			ENG_CORE_ERROR("Could not open shader file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::Preprocess(const std::string& source)
	{
		ENG_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		// Find type token
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		// Repeat finding it until last occurrence
		while (pos != std::string::npos)
		{
			// Make sure there is actual source after the type token
			size_t eol = source.find_first_of("\r\n", pos);
			ENG_CORE_ASSERT(eol != std::string::npos, "Syntax error");

			// Extract shader type
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			ENG_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified!");

			// If there is no other type token, take the string till eof. Otherwise take it till the next type token.
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			ENG_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);
			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_vulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_filePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*) data.data(), size);
			} else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_filePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ENG_CORE_ERROR(module.GetErrorMessage());
					ENG_CORE_ASSERT(false);
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*) data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_openGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_openGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_vulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_filePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*) data.data(), size);
			} else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_openGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_openGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_filePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ENG_CORE_ERROR(module.GetErrorMessage());
					ENG_CORE_ASSERT(false);
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*) data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_openGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			ENG_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_filePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_rendererID = program;
	}

	void OpenGLShader::CreateProgramForAmdGpu()
	{
		GLuint program = glCreateProgram();

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
		std::filesystem::path shaderFilePath = m_filePath;
		std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + ".cached_opengl.pgr");
		std::ifstream in(cachedPath, std::ios::ate | std::ios::binary);

		if (in.is_open())
		{
			auto size = in.tellg();
			in.seekg(0);

			auto& data = std::vector<char>(size);
			uint32_t format = 0;
			in.read((char*) &format, sizeof(uint32_t));
			in.read((char*) data.data(), size);
			glProgramBinary(program, format, data.data(), data.size());

			bool linked = Utils::VerifyProgramLink(program);
			if (!linked)
				return;
		} else
		{
			std::array<uint32_t, 2> glShaderIDs;
			CompileOpenGLBinariesForAmdGpu(program, glShaderIDs);
			glLinkProgram(program);

			bool linked = Utils::VerifyProgramLink(program);

			if (linked)
			{
				// Save program data
				GLint formats = 0;
				glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
				ENG_CORE_ASSERT(formats > 0, "Driver does not support binary format");
				Utils::CreateCacheDirectoryIfNeeded();
				GLint length = 0;
				glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);
				auto shaderData = std::vector<char>(length);
				uint32_t format = 0;
				glGetProgramBinary(program, length, nullptr, &format, shaderData.data());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					out.write((char*) &format, sizeof(uint32_t));
					out.write(shaderData.data(), shaderData.size());
					out.flush();
					out.close();
				}
			}

			for (auto& id : glShaderIDs)
				glDetachShader(program, id);
		}

		m_rendererID = program;
	}

	void OpenGLShader::CompileOpenGLBinariesForAmdGpu(GLenum& program, std::array<uint32_t, 2>& glShaderIDs)
	{
		int glShaderIDIndex = 0;
		for (auto&& [stage, spirv] : m_vulkanSPIRV)
		{
			spirv_cross::CompilerGLSL glslCompiler(spirv);
			auto& source = glslCompiler.compile();

			uint32_t shader;

			shader = glCreateShader(stage);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			int isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				int maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<char> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				ENG_CORE_ERROR("{0}", infoLog.data());
				ENG_CORE_ASSERT(false, "[OpenGL] Shader compilation failure!");
				return;
			}
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		ENG_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_filePath);
		ENG_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		ENG_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		ENG_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			ENG_CORE_TRACE("  {0}", resource.name);
			ENG_CORE_TRACE("    Size = {0}", bufferSize);
			ENG_CORE_TRACE("    Binding = {0}", binding);
			ENG_CORE_TRACE("    Members = {0}", memberCount);
		}
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		ENG_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		ENG_PROFILE_FUNCTION();

		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		ENG_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		ENG_PROFILE_FUNCTION();

		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		ENG_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		ENG_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		ENG_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
