#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

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
	Buffer();
	~Buffer();

	void Init();
	void Cleanup(VkDevice i_device);

	void CreateBuffer(VkDevice i_device, VkPhysicalDevice i_physicalDevice, VkDeviceSize i_size, VkBufferUsageFlags i_usage, VkMemoryPropertyFlags i_properties, VkBuffer& i_buffer, VkDeviceMemory& i_bufferMemory);
	void CreateVertexBuffer(VkDevice i_device, VkPhysicalDevice i_physicalDevice, VkCommandPool i_commandPool, VkQueue i_submitQueue);
	VertexBufferDescriptor GetVertexBufferDescriptor();

private:
	uint32_t FindMemoryType(VkPhysicalDevice i_physicalDevice, uint32_t i_typeFilter, VkMemoryPropertyFlags i_properties);
	void CopyBuffer(VkDevice i_device, VkCommandPool i_commandPool, VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size, VkQueue i_submitQueue);

private:
	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;

	const std::vector<Vertex> m_vertices = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI

