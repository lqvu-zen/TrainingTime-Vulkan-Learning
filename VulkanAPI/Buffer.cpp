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

void Buffer::CreateVertexBuffer(VkDevice i_device, VkPhysicalDevice i_physicalDevice)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(m_vertices[0]) * m_vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.flags = 0;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	if (vkCreateBuffer(i_device, &bufferInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(i_device, m_vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(i_physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(i_device, &allocInfo, nullptr, &m_vertexBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(i_device, m_vertexBuffer, m_vertexBufferMemory, 0);

	void* data;
	vkMapMemory(i_device, m_vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, m_vertices.data(), (size_t)bufferInfo.size); //COPY
	vkUnmapMemory(i_device, m_vertexBufferMemory);

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
}// VulkanAPI