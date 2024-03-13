#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class CommandBuffer
{
public:
	CommandBuffer(const int i_size, VkDevice i_device, VkCommandPool i_commandPool);
	~CommandBuffer();

	std::vector<VkCommandBuffer> GetCommandBuffers();
	VkCommandBuffer GetCommandBufferAtFrame(const int i_frame);

private:
	std::vector<VkCommandBuffer> m_commandBuffers;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI

