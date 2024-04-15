#include "Buffer.h"

#include <stdexcept>

#include <VulkanAPI/Device.h>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

VulkanAPI::Buffer::Buffer(std::unique_ptr<Device>& i_deviceManager, VkCommandPool i_commandPool)
	: m_deviceManager(i_deviceManager)
	, m_device(i_deviceManager->GetDevice())
	, m_commandPool(i_commandPool)
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

void Buffer::Cleanup()
{
	vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
	vkFreeMemory(m_device, m_indexBufferMemory, nullptr);

	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
	vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	{
		memcpy(data, m_vertices.data(), bufferSize); //COPY
	}
	vkUnmapMemory(m_device, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

	CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize, m_deviceManager->GetGraphicsQueue());

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	{
		memcpy(data, m_indices.data(), (size_t)bufferSize);
	}
	vkUnmapMemory(m_device, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

	CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize, m_deviceManager->GetGraphicsQueue());

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
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

VkBuffer Buffer::GetIndexBuffer()
{
	return m_indexBuffer;
}

///////////////////////////////////////////////////////////////////////////////

uint32_t Buffer::GetIndicesSize()
{
	return static_cast<uint32_t>(m_indices.size());
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::CreateBuffer(VkDeviceSize i_size, VkBufferUsageFlags i_usage, VkMemoryPropertyFlags i_properties, VkBuffer& i_buffer, VkDeviceMemory& i_bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = i_size;
	bufferInfo.usage = i_usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &i_buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_device, i_buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = m_deviceManager->GetMemoryType(memRequirements.memoryTypeBits, i_properties);

	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &i_bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_device, i_buffer, i_bufferMemory, 0);
}

///////////////////////////////////////////////////////////////////////////////

void Buffer::CopyBuffer(VkBuffer i_srcBuffer, VkBuffer i_dstBuffer, VkDeviceSize i_size, VkQueue i_submitQueue)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

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

	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI