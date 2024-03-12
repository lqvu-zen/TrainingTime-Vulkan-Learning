#include "RenderPass.h"

#include <stdexcept>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

RenderPass::RenderPass(VkDevice i_device, VkAttachmentDescription i_colorAttachment, VkSubpassDescription i_subpass, VkSubpassDependency i_dependency)
	: m_device(i_device)
{
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &i_colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &i_subpass;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &i_dependency;

	if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

RenderPass::~RenderPass()
{
}

VkRenderPass RenderPass::GetRenderPass()
{
	return m_renderPass;
}

void RenderPass::CleanUp()
{
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);
}

///////////////////////////////////////////////////////////////////////////////
}