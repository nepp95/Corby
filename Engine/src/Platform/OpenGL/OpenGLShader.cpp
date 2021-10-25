#include "engpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {
	static GLenum ShaderTypeFromString(const std::string& type) {
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment")
			return GL_FRAGMENT_SHADER;

		ENG_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath) {
		std::string source = readFile(filepath);
		auto shaderSources = preprocess(source);
		compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		compile(sources);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::bind() const {
		glUseProgram(m_rendererID);
	}

	void OpenGLShader::unbind() const {
		glUseProgram(0);
	}

	std::string OpenGLShader::readFile(const std::string& filepath) {
		std::ifstream in(filepath, std::ios::in, std::ios::binary);
		std::string result;

		if (in) {
			// Find out the size of the file and resize the string accordingly
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());

			// Put the file contents in the string
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());

			// Close it up
			in.close();
		}
		else {
			ENG_CORE_ERROR("Could not open shader file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preprocess(const std::string& source) {
		std::unordered_map<GLenum, std::string> shaderSources;

		// Find type token
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		// Repeat finding it until last occurrence
		while (pos != std::string::npos) {
			// Make sure there is actual source after the type token
			size_t eol = source.find_first_of("\r\n", pos);
			ENG_CORE_ASSERT(eol != std::string::npos, "Syntax error");

			// Extract shader type
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			ENG_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!");

			// If there is no other type token, take the string till eof. Otherwise take it till the next type token.
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::compile(std::unordered_map<GLenum, std::string> shaderSources) {
		GLuint program = glCreateProgram();
		std::vector<GLenum> glShaderIDs(shaderSources.size());

		for (auto& kv : shaderSources) {
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			// Set the vertex shader source
			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			// Compile vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// maxLength includes NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// Shader is not needed anymore
				glDeleteShader(shader);

				ENG_CORE_ERROR("{0}", infoLog.data());
				ENG_CORE_ASSERT(false, "Vertex shader compilation failure!");

				break;
			}

			glAttachShader(program, shader);
			glShaderIDs.push_back(shader);
		}

		// Create program
		m_rendererID = program;

		// Link our program
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);

		if (!isLinked) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// maxLength includes NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// Program is not needed anymore
			glDeleteProgram(program);

			// We don't want to leak shaders
			for (auto id : glShaderIDs)
				glDeleteShader(id);

			ENG_CORE_ERROR("{0}", infoLog.data());
			ENG_CORE_ASSERT(false, "Shader link failure!");

			return;
		}

		// Detach shaders because the program is now compiled and linked
		for (auto id : glShaderIDs)
			glDetachShader(program, id);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformFloat(const std::string& name, float value) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}