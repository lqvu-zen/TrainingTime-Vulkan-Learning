#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <vector>

namespace VulkanAPI
{
class Device;
}

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

///////////////////////////////////////////////////////////////////////////////

struct VertexBufferDescriptor
{
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class Buffer
{
public:
	Buffer(std::unique_ptr<VulkanAPI::Device>& i_deviceManager, VkCommandPool i_commandPool);
	~Buffer();

	void Init();
	void Cleanup();

	void CreateVertexBuffer();
	void CreateIndexBuffer();
	VertexBufferDescriptor GetVertexBufferDescriptor();
	VkBuffer GetIndexBuffer();
	uint32_t GetIndicesSize();

private:
	void CreateBuffer(VkDeviceSize i_size, VkBufferUsageFlags i_usage, VkMemoryPropertyFlags i_properties, VkBuffer& i_buffer, VkDeviceMemory& i_bufferMemory);
	void CopyBuffer(VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size, VkQueue i_submitQueue);

private:
	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;
	VkBuffer m_indexBuffer;
	VkDeviceMemory m_indexBufferMemory;

	std::unique_ptr<Device>& m_deviceManager;
	VkDevice m_device;
	VkCommandPool m_commandPool;

	const std::vector<Vertex> m_vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	const std::vector<uint16_t> m_indices = {
		0, 1, 2, 2, 3, 0
	};
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI

