#pragma once

#include <vulkan/vulkan.h>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class RenderPass
{
public:
	RenderPass(VkDevice i_device, VkAttachmentDescription i_colorAttachment, VkSubpassDescription i_subpass);
	~RenderPass();

	VkDevice GetRenderPass();
	void CleanUp();

private:
	VkRenderPass m_renderPass;

	VkDevice m_device;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI

