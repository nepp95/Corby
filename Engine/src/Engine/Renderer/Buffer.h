#pragma once

namespace Engine
{
	// -----------------------------------------
	//
	//    ShaderDataType
	//    ShaderDataTypeSize
	//
	// -----------------------------------------
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static unsigned int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;
		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;
		case ShaderDataType::Bool:		return 1;
		}

		ENG_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	// -----------------------------------------
	//
	//    BufferElement
	//    BufferLayout
	//
	// -----------------------------------------
	struct BufferElement
	{
		ShaderDataType type;
		std::string name;
		uint32_t size;
		size_t offset;
		bool normalized;

		BufferElement() {}
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) : name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

		unsigned int getComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Mat3:		return 3;
			case ShaderDataType::Mat4:		return 4;
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Bool:		return 1;
			}

			ENG_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(std::initializer_list<BufferElement> elements);

		unsigned int getStride() const { return m_stride; }
		const std::vector<BufferElement>& getElements() const { return m_elements; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:
		void calculateOffsetsAndStride();

	private:
		std::vector<BufferElement> m_elements;
		unsigned int m_stride;
	};

	// -----------------------------------------
	//
	//    VertexBuffer
	//
	// -----------------------------------------
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		virtual const BufferLayout& getLayout() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> create(uint32_t size);
		static Ref<VertexBuffer> create(float* vertices, unsigned int size);
	};

	// -----------------------------------------
	//
	//    IndexBuffer
	//
	// -----------------------------------------
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual unsigned int getCount() const = 0;

		static Ref<IndexBuffer> create(unsigned int* indices, unsigned int count);
	};
}
