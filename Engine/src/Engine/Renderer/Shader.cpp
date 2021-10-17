#include "engpch.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {
	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
		// Create empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Set the vertex shader source
		const char* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile vertex shader
		glCompileShader(vertexShader);

		int isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

		if (!isCompiled) {
			int maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// maxLength includes NULL character
			std::vector<char> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// Shader is not needed anymore
			glDeleteShader(vertexShader);

			ENGINE_CORE_ERROR("{0}", infoLog.data());
			ENGINE_CORE_ASSERT(false, "Vertex shader compilation fail!");

			return;
		}

		// Create empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Set the fragment shader source
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);

		if (!isCompiled) {
			int maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// maxLength includes NULL character
			std::vector<char> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// Shader is not needed anymore
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			ENGINE_CORE_ERROR("{0}", infoLog.data());
			ENGINE_CORE_ASSERT(false, "Vertex shader compilation fail!");

			return;
		}

		// Create program
		m_rendererID = glCreateProgram();
		unsigned int program = m_rendererID;

		// Attach shader to program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		// Link our program
		glLinkProgram(program);

		int isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);

		if (!isLinked) {
			int maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// maxLength includes NULL character
			std::vector<char> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// Program is not needed anymore
			glDeleteProgram(program);

			// We don't want to leak shaders
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			ENGINE_CORE_ERROR("{0}", infoLog.data());
			ENGINE_CORE_ASSERT(false, "Shader link fail!");

			return;
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}

	Shader::~Shader() {
		glDeleteProgram(m_rendererID);
	}

	void Shader::bind() const {
		glUseProgram(m_rendererID);
	}

	void Shader::unbind() const {
		glUseProgram(0);
	}

	void Shader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}