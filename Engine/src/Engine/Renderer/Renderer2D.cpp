#include "engpch.h"
#include "Renderer2D.h"

#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
	struct Renderer2DStorage {
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> textureShader;
		Ref<Texture2D> whiteTexture;
	};

	// TODO: make ref
	static Ref<Renderer2DStorage> s_data;

	void Renderer2D::init() {
		ENG_PROFILE_FUNCTION();

		s_data = createRef<Renderer2DStorage>();
		s_data->quadVertexArray = VertexArray::create();

		// VertexBuffer
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Ref<VertexBuffer> squareVB = VertexBuffer::create(squareVertices, sizeof(squareVertices));
		squareVB->setLayout({
			{ ShaderDataType::Float3, "a_position" },
			{ ShaderDataType::Float2, "a_texCoord" }
			});
		s_data->quadVertexArray->addVertexBuffer(squareVB);

		// IndexBuffer
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_data->quadVertexArray->setIndexBuffer(squareIB);

		s_data->whiteTexture = Texture2D::create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_data->whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		s_data->textureShader = Shader::create("assets/shaders/Texture.glsl");
		s_data->textureShader->bind();
		s_data->textureShader->setInt("u_texture", 0);
	}

	void Renderer2D::shutdown() {
		ENG_PROFILE_FUNCTION();
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera) {
		ENG_PROFILE_FUNCTION();

		s_data->textureShader->bind();
		s_data->textureShader->setMat4("u_viewProjection", camera.getViewProjectionMatrix());
	}

	void Renderer2D::endScene() {
		ENG_PROFILE_FUNCTION();
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
		drawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
		ENG_PROFILE_FUNCTION();

		s_data->textureShader->setFloat4("u_color", color);
		s_data->whiteTexture->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
		s_data->textureShader->setMat4("u_transform", transform);

		s_data->quadVertexArray->bind();
		RenderCommand::drawIndexed(s_data->quadVertexArray);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture) {
		drawQuad({ position.x, position.y, 0.0f }, size, texture);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture) {
		ENG_PROFILE_FUNCTION();

		s_data->textureShader->setFloat4("u_color", glm::vec4(1.0f));
		texture->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_data->textureShader->setMat4("u_transform", transform);

		s_data->quadVertexArray->bind();
		RenderCommand::drawIndexed(s_data->quadVertexArray);
	}
}