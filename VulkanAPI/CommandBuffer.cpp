#include "CommandBuffer.h"

#include <stdexcept>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

CommandBuffer::CommandBuffer(VkDevice i_device, VkCommandPool i_commandPool)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = i_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(i_device, &allocInfo, &m_commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

CommandBuffer::~CommandBuffer()
{
}

VkCommandBuffer CommandBuffer::GetCommandBuffer()
{
	return m_commandBuffer;
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI
