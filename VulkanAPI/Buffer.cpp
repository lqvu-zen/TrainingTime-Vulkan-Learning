#include "Buffer.h"

#include <stdexcept>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

VulkanAPI::Buffer::Buffer()
{
}

///////////////////////////////////////////////////////////////////////////////

VulkanAPI::Buffer::~Buffer()
{
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::Init()
{
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::Cleanup(VkDevice i_device)
{
	vkDestroyBuffer(i_device, m_vertexBuffer, nullptr);
	vkFreeMemory(i_device, m_vertexBufferMemory, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::CreateBuffer(VkDevice i_device, VkPhysicalDevice i_physicalDevice, VkDeviceSize i_size, VkBufferUsageFlags i_usage, VkMemoryPropertyFlags i_properties, VkBuffer& i_buffer, VkDeviceMemory& i_bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = i_size;
	bufferInfo.usage = i_usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(i_device, &bufferInfo, nullptr, &i_buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(i_device, i_buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(i_physicalDevice, memRequirements.memoryTypeBits, i_properties);

	if (vkAllocateMemory(i_device, &allocInfo, nullptr, &i_bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(i_device, i_buffer, i_bufferMemory, 0);
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::CreateVertexBuffer(VkDevice i_device, VkPhysicalDevice i_physicalDevice, VkCommandPool i_commandPool, VkQueue i_submitQueue)
{
	VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(i_device, i_physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(i_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	{
		memcpy(data, m_vertices.data(), bufferSize); //COPY
	}
	vkUnmapMemory(i_device, stagingBufferMemory);

	CreateBuffer(i_device, i_physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

	CopyBuffer(i_device, i_commandPool, stagingBuffer, m_vertexBuffer, bufferSize, i_submitQueue);

	vkDestroyBuffer(i_device, stagingBuffer, nullptr);
	vkFreeMemory(i_device, stagingBufferMemory, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

VertexBufferDescriptor Buffer::GetVertexBufferDescriptor()
{
	VertexBufferDescriptor vertexBufferDescriptor;
	vertexBufferDescriptor.vertexBuffers.push_back(m_vertexBuffer);
	vertexBufferDescriptor.offsets.push_back(0);

	return vertexBufferDescriptor;
}

///////////////////////////////////////////////////////////////////////////////

uint32_t Buffer::FindMemoryType(VkPhysicalDevice i_physicalDevice, uint32_t i_typeFilter, VkMemoryPropertyFlags i_properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(i_physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if ((i_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & i_properties) == i_properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::CopyBuffer(VkDevice i_device, VkCommandPool i_commandPool, VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size, VkQueue i_submitQueue)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = i_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(i_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	{
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = i_size;
		vkCmdCopyBuffer(commandBuffer, i_srcBuffer, i_dstBuffer, 1, &copyRegion);
	}
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(i_submitQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(i_submitQueue);

	vkFreeCommandBuffers(i_device, i_commandPool, 1, &commandBuffer);
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI