#include "CommandBuffer.h"

#include <stdexcept>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

CommandBuffer::CommandBuffer(const int i_size, VkDevice i_device, VkCommandPool i_commandPool)
{
	m_commandBuffers.resize(i_size);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = i_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if (vkAllocateCommandBuffers(i_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

CommandBuffer::~CommandBuffer()
{
}

std::vector<VkCommandBuffer> CommandBuffer::GetCommandBuffers()
{
	return m_commandBuffers;
}

VkCommandBuffer CommandBuffer::GetCommandBufferAtFrame(const int i_frame)
{
	return m_commandBuffers[i_frame];
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI
