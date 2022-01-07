#include "engpch.h"
#include "Renderer2D.h"

#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/UniformBuffer.h"
#include "Engine/Renderer/VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_data;

	void Renderer2D::Init()
	{
		ENG_PROFILE_FUNCTION();

		// Quad vertex array + buffer
		s_data.QuadVertexArray = VertexArray::Create();
		s_data.QuadVertexBuffer = VertexBuffer::Create(s_data.MaxVertices * sizeof(QuadVertex));

		s_data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"		},
			{ ShaderDataType::Float4, "a_Color"			},
			{ ShaderDataType::Float2, "a_TexCoord"		},
			{ ShaderDataType::Float,  "a_TexIndex"		},
			{ ShaderDataType::Float,  "a_TilingFactor"	},
			{ ShaderDataType::Int,    "a_EntityID"		}
			});
		s_data.QuadVertexArray->AddVertexBuffer(s_data.QuadVertexBuffer);
		s_data.QuadVertexBufferBase = new QuadVertex[s_data.MaxVertices];

		// Quad index buffer
		uint32_t* QuadIndices = new uint32_t[s_data.MaxIndices];
		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_data.MaxIndices; i += 6)
		{
			QuadIndices[i + 0] = offset + 0;
			QuadIndices[i + 1] = offset + 1;
			QuadIndices[i + 2] = offset + 2;

			QuadIndices[i + 3] = offset + 2;
			QuadIndices[i + 4] = offset + 3;
			QuadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(QuadIndices, s_data.MaxIndices);
		s_data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] QuadIndices;

		// Circle vertex array + buffer
		s_data.CircleVertexArray = VertexArray::Create();
		s_data.CircleVertexBuffer = VertexBuffer::Create(s_data.MaxVertices * sizeof(CircleVertex));

		s_data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition"	},
			{ ShaderDataType::Float3, "a_LocalPosition"	},
			{ ShaderDataType::Float4, "a_Color"			},
			{ ShaderDataType::Float,  "a_Thickness"		},
			{ ShaderDataType::Float,  "a_Fade"			},
			{ ShaderDataType::Int,    "a_EntityID"		}
			});

		s_data.CircleVertexArray->AddVertexBuffer(s_data.CircleVertexBuffer);
		s_data.CircleVertexArray->SetIndexBuffer(quadIB); // Use quadIB is valid.
		s_data.CircleVertexBufferBase = new CircleVertex[s_data.MaxVertices];

		// Line vertex array + buffer
		s_data.LineVertexArray = VertexArray::Create();
		s_data.LineVertexBuffer = VertexBuffer::Create(s_data.MaxVertices * sizeof(LineVertex));

		s_data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float4, "a_Color"		},
			{ ShaderDataType::Int, "a_EntityID"		}
			});

		s_data.LineVertexArray->AddVertexBuffer(s_data.LineVertexBuffer);
		s_data.LineVertexBufferBase = new LineVertex[s_data.MaxVertices];

		// White texture slot
		s_data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_data.MaxTextureSlots; i++)
			samplers[i] = i;

		// Create shaders
		s_data.QuadShader = Shader::Create("assets/shaders/2DQuad.glsl");
		s_data.CircleShader = Shader::Create("assets/shaders/2DCircle.glsl");
		s_data.LineShader = Shader::Create("assets/shaders/2DLine.glsl");

		// Set first texture slot to 0
		s_data.TextureSlots[0] = s_data.WhiteTexture;

		s_data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		ENG_PROFILE_FUNCTION();

		s_data.QuadIndexCount = 0;
		s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

		s_data.CircleIndexCount = 0;
		s_data.CircleVertexBufferPtr = s_data.CircleVertexBufferBase;

		s_data.LineVertexCount = 0;
		s_data.LineVertexBufferPtr = s_data.LineVertexBufferBase;

		s_data.TextureSlotIndex = 1;

		delete[] s_data.QuadVertexBufferBase;
		delete[] s_data.CircleVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		ENG_PROFILE_FUNCTION();

		s_data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_data.CameraUniformBuffer->SetData(&s_data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		ENG_PROFILE_FUNCTION();

		s_data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_data.CameraUniformBuffer->SetData(&s_data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		ENG_PROFILE_FUNCTION();

		s_data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_data.CameraUniformBuffer->SetData(&s_data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		ENG_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::Flush()
	{
		// If there is no quads to draw
		if (s_data.QuadIndexCount)
		{
			// Calculate data size
			uint32_t dataSize = (uint32_t) ((uint8_t*) s_data.QuadVertexBufferPtr - (uint8_t*) s_data.QuadVertexBufferBase);
			s_data.QuadVertexBuffer->SetData(s_data.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_data.TextureSlotIndex; i++)
				s_data.TextureSlots[i]->Bind(i);

			// Create draw call
			s_data.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_data.QuadVertexArray, s_data.QuadIndexCount);
			s_data.Stats.DrawCalls++;
		}

		if (s_data.CircleIndexCount)
		{
			// Calculate data size
			uint32_t dataSize = (uint32_t) ((uint8_t*) s_data.CircleVertexBufferPtr - (uint8_t*) s_data.CircleVertexBufferBase);
			s_data.CircleVertexBuffer->SetData(s_data.CircleVertexBufferBase, dataSize);

			// Create draw call
			s_data.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_data.CircleVertexArray, s_data.CircleIndexCount);
			s_data.Stats.DrawCalls++;
		}

		if (s_data.LineVertexCount)
		{
			// Calculate data size
			uint32_t dataSize = (uint32_t) ((uint8_t*) s_data.LineVertexBufferPtr - (uint8_t*) s_data.LineVertexBufferBase);
			s_data.LineVertexBuffer->SetData(s_data.LineVertexBufferBase, dataSize);

			// Create draw call
			s_data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_data.LineWidth);
			RenderCommand::DrawLines(s_data.LineVertexArray, s_data.LineVertexCount);
			s_data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		ENG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		ENG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		ENG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		ENG_PROFILE_FUNCTION();

		constexpr size_t QuadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < QuadVertexCount; i++)
		{
			s_data.QuadVertexBufferPtr->Position = transform * s_data.QuadVertexPositions[i];
			s_data.QuadVertexBufferPtr->Color = color;
			s_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_data.QuadVertexBufferPtr->EntityID = entityID;
			s_data.QuadVertexBufferPtr++;
		}

		s_data.QuadIndexCount += 6;
		s_data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		ENG_PROFILE_FUNCTION();

		constexpr size_t QuadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_data.TextureSlotIndex; i++)
		{
			if (*s_data.TextureSlots[i] == *texture)
			{
				textureIndex = (float) i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float) s_data.TextureSlotIndex;
			s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
			s_data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < QuadVertexCount; i++)
		{
			s_data.QuadVertexBufferPtr->Position = transform * s_data.QuadVertexPositions[i];
			s_data.QuadVertexBufferPtr->Color = tintColor;
			s_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_data.QuadVertexBufferPtr->EntityID = entityID;
			s_data.QuadVertexBufferPtr++;
		}

		s_data.QuadIndexCount += 6;

		s_data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		ENG_PROFILE_FUNCTION();

		constexpr size_t QuadVertexCount = 4;
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_data.TextureSlotIndex; i++)
		{
			if (*s_data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float) i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float) s_data.TextureSlotIndex;
			s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
			s_data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < QuadVertexCount; i++)
		{
			s_data.QuadVertexBufferPtr->Position = transform * s_data.QuadVertexPositions[i];
			s_data.QuadVertexBufferPtr->Color = tintColor;
			s_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_data.QuadVertexBufferPtr->EntityID = entityID;
			s_data.QuadVertexBufferPtr++;
		}

		s_data.QuadIndexCount += 6;
		s_data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		ENG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		ENG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		ENG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		ENG_PROFILE_FUNCTION();

		// TODO: implement for circles
		//if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
		//	  NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_data.CircleVertexBufferPtr->WorldPosition = transform * s_data.QuadVertexPositions[i];
			s_data.CircleVertexBufferPtr->LocalPosition = s_data.QuadVertexPositions[i] * 2.0f;
			s_data.CircleVertexBufferPtr->Color = color;
			s_data.CircleVertexBufferPtr->Thickness = thickness;
			s_data.CircleVertexBufferPtr->Fade = fade;
			s_data.CircleVertexBufferPtr->EntityID = entityID;
			s_data.CircleVertexBufferPtr++;
		}

		s_data.CircleIndexCount += 6;
		s_data.Stats.QuadCount++; // TODO: Add a Circle Count?
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_data.LineVertexBufferPtr->Position = p0;
		s_data.LineVertexBufferPtr->Color = color;
		s_data.LineVertexBufferPtr->EntityID = entityID;
		s_data.LineVertexBufferPtr++;

		s_data.LineVertexBufferPtr->Position = p1;
		s_data.LineVertexBufferPtr->Color = color;
		s_data.LineVertexBufferPtr->EntityID = entityID;
		s_data.LineVertexBufferPtr++;

		s_data.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_data.LineWidth = width;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_data.Stats;
	}

	void Renderer2D::StartBatch()
	{
		s_data.QuadIndexCount = 0;
		s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

		s_data.CircleIndexCount = 0;
		s_data.CircleVertexBufferPtr = s_data.CircleVertexBufferBase;

		s_data.LineVertexCount = 0;
		s_data.LineVertexBufferPtr = s_data.LineVertexBufferBase;

		s_data.TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}
}
