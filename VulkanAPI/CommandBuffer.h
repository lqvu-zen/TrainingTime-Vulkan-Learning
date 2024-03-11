#pragma once

#include <vulkan/vulkan.h>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class CommandBuffer
{
public:
	CommandBuffer(VkDevice i_device, VkCommandPool i_commandPool);
	~CommandBuffer();

	VkCommandBuffer GetCommandBuffer();

private:
	VkCommandBuffer m_commandBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI

