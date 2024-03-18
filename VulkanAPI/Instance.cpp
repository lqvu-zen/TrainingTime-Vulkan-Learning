#include "Instance.h"

#define NOMINMAX   
#include <iostream>
#include <stdexcept>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp

#include "ValidationLayer.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WindowSurface.h"
#include "RenderPass.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "CommandBuffer.h"

#include "Window.h"
#include "FileSystem.h"

//Extensions
VkResult CreateDebugUtilsMessengerEXT(VkInstance i_instance, const VkDebugUtilsMessengerCreateInfoEXT* i_createInfo, const VkAllocationCallbacks* i_allocator, VkDebugUtilsMessengerEXT* i_debugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(i_instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(i_instance, i_createInfo, i_allocator, i_debugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance i_instance, VkDebugUtilsMessengerEXT i_debugMessenger, const VkAllocationCallbacks* i_allocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(i_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(i_instance, i_debugMessenger, i_allocator);
	}
}
//Extensions

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

Instance::Instance(std::unique_ptr<ValidationLayer>& i_validationLayer, std::unique_ptr<Window>& i_window, std::unique_ptr<FileSystem>& i_fileSystem)
	: m_instance(nullptr)
	, m_validationLayer(i_validationLayer)
	, m_window(i_window)
	, m_fileSystem(i_fileSystem)
{
	if (m_validationLayer->IsEnable() && !m_validationLayer->CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "My Vulkan Training";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	const std::vector<const char*> validationLayers = m_validationLayer->GetLayers();

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (m_validationLayer->IsEnable()) {

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	VkResult createInstanceResult = vkCreateInstance(&createInfo, nullptr, &m_instance);
	if (createInstanceResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

///////////////////////////////////////////////////////////////////////////////

Instance::~Instance()
{
	Cleanup();
}

///////////////////////////////////////////////////////////////////////////////

VkInstance Instance::GetInstance()
{
	return m_instance;
}

///////////////////////////////////////////////////////////////////////////////

void Instance::SetupDebugMessenger()
{
	if (!m_validationLayer->IsEnable()) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateSurface()
{
	m_windowSurface = std::make_unique<WindowSurface>(m_instance, m_window->GetWindow());
}

///////////////////////////////////////////////////////////////////////////////

void Instance::PickPhysicalDevice()
{
	m_physicalDevice = std::make_unique<PhysicalDevice>(m_instance, m_windowSurface->GetSurface());
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateLogicalDevice()
{
	m_logicalDevice = std::make_unique<LogicalDevice>(m_validationLayer, m_physicalDevice);
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = m_physicalDevice->GetSwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_windowSurface->GetSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = m_physicalDevice->GetQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_logicalDevice->GetDevice(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_logicalDevice->GetDevice(), m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_logicalDevice->GetDevice(), m_swapChain, &imageCount, m_swapChainImages.data());

	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size());

	for (size_t i = 0; i < m_swapChainImages.size(); ++i) 
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_logicalDevice->GetDevice(), &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	m_renderPass = std::make_unique<RenderPass>(m_logicalDevice->GetDevice(), colorAttachment, subpass, dependency);
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateGraphicsPipeline()
{
	//Shader
	auto vertShaderCode = m_fileSystem->ReadFile("Shaders/vert.spv");
	auto fragShaderCode = m_fileSystem->ReadFile("Shaders/frag.spv");

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = 
		{ 
			vertShaderStageInfo, 
			fragShaderStageInfo 
		};

	//Vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	//Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//Viewport state
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	//Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	//Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	//Depth and stencil
	//TODO

	//Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	//Dynamic state
	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	//Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(m_logicalDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	//Graphics Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass->GetRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(m_logicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	//Finish
	vkDestroyShaderModule(m_logicalDevice->GetDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(m_logicalDevice->GetDevice(), vertShaderModule, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateFramebuffers()
{
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

	for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = 
			{
				m_swapChainImageViews[i]
			};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass->GetRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_logicalDevice->GetDevice(), &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = m_physicalDevice->GetQueueFamilies();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(m_logicalDevice->GetDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateCommandBuffers()
{
	m_commandBuffer = std::make_unique<CommandBuffer>(K_MAX_FRAMES_IN_FLIGHT, m_logicalDevice->GetDevice(), m_commandPool);
}

///////////////////////////////////////////////////////////////////////////////

void Instance::DrawFrame()
{
	//Waiting for the previous frame
	vkWaitForFences(m_logicalDevice->GetDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	//Acquiring an image from the swap chain
	uint32_t imageIndex;
	VkResult acquireResult = vkAcquireNextImageKHR(m_logicalDevice->GetDevice(), m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Only reset the fence if we are submitting work
	vkResetFences(m_logicalDevice->GetDevice(), 1, &m_inFlightFences[m_currentFrame]);

	//Recording the command buffer
	vkResetCommandBuffer(m_commandBuffer->GetCommandBufferAtFrame(m_currentFrame), 0);
	RecordCommandBuffer(m_commandBuffer->GetCommandBufferAtFrame(m_currentFrame), imageIndex);

	//Submitting the command buffer
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	VkCommandBuffer commandBuffer = m_commandBuffer->GetCommandBufferAtFrame(m_currentFrame);
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_logicalDevice->GetGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//Presentation
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr; // Optional

	VkResult queuePresentResult = vkQueuePresentKHR(m_logicalDevice->GetPresentQueue(), &presentInfo);

	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR || m_window->IsFramebufferResized())
	{
		m_window->SetFramebufferResized(false);
		RecreateSwapChain();
	}
	else if (queuePresentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	//After present
	m_currentFrame = (m_currentFrame + 1) % K_MAX_FRAMES_IN_FLIGHT;
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateSyncObjects()
{
	m_imageAvailableSemaphores.resize(K_MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(K_MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(K_MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < K_MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_logicalDevice->GetDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Instance::RecreateSwapChain()
{
	while (m_window->IsMinimizing()) 
	{
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_logicalDevice->GetDevice());

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateFramebuffers();
}


///////////////////////////////////////////////////////////////////////////////

void Instance::CleanupSwapChain()
{
	for (auto framebuffer : m_swapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_logicalDevice->GetDevice(), framebuffer, nullptr);
	}

	for (auto imageView : m_swapChainImageViews)
	{
		vkDestroyImageView(m_logicalDevice->GetDevice(), imageView, nullptr);
	}

	vkDestroySwapchainKHR(m_logicalDevice->GetDevice(), m_swapChain, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Instance::Cleanup()
{
	CleanupSwapChain();

	vkDestroyPipeline(m_logicalDevice->GetDevice(), m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_logicalDevice->GetDevice(), m_pipelineLayout, nullptr);

	m_renderPass->Cleanup();

	for (size_t i = 0; i < K_MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_logicalDevice->GetDevice(), m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_logicalDevice->GetDevice(), m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_logicalDevice->GetDevice(), m_inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_logicalDevice->GetDevice(), m_commandPool, nullptr);

	m_logicalDevice->Cleanup();

	if (m_validationLayer->IsEnable())
	{
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}

	m_windowSurface->Cleanup();

	vkDestroyInstance(m_instance, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Instance::Shutdown()
{
	vkDeviceWaitIdle(m_logicalDevice->GetDevice());
}

///////////////////////////////////////////////////////////////////////////////

std::vector<const char*> Instance::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_validationLayer->IsEnable())
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

///////////////////////////////////////////////////////////////////////////////

void Instance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& i_createInfo)
{
	i_createInfo = {};
	i_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	i_createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	i_createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	i_createInfo.pfnUserCallback = DebugCallback;
	i_createInfo.pUserData = nullptr; // Optional
}

///////////////////////////////////////////////////////////////////////////////

VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT i_messageSeverity, VkDebugUtilsMessageTypeFlagsEXT i_messageType, const VkDebugUtilsMessengerCallbackDataEXT* i_callbackData, void* i_userData)
{
	std::cerr << "validation layer: " << i_callbackData->pMessage << std::endl;

	return VK_FALSE;
}

///////////////////////////////////////////////////////////////////////////////

VkSurfaceFormatKHR Instance::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& i_availableFormats) {
	for (const auto& availableFormat : i_availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return i_availableFormats[0];
}

///////////////////////////////////////////////////////////////////////////////

VkPresentModeKHR Instance::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& i_availablePresentModes) {
	for (const auto& availablePresentMode : i_availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

///////////////////////////////////////////////////////////////////////////////

VkExtent2D Instance::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& i_capabilities)
{
	if (i_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
	{
		return i_capabilities.currentExtent;
	}
	else 
	{
		int width, height;
		glfwGetFramebufferSize(m_window->GetWindow(), &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, i_capabilities.minImageExtent.width, i_capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, i_capabilities.minImageExtent.height, i_capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

VkShaderModule Instance::CreateShaderModule(const std::vector<char>& i_code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = i_code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(i_code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_logicalDevice->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

///////////////////////////////////////////////////////////////////////////////

void Instance::RecordCommandBuffer(VkCommandBuffer i_commandBuffer, uint32_t i_imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(i_commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	//Starting render pass
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass->GetRenderPass();
	renderPassInfo.framebuffer = m_swapChainFramebuffers[i_imageIndex];
	
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(i_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//Bind Pipeline
	vkCmdBindPipeline(i_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapChainExtent.width);
	viewport.height = static_cast<float>(m_swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(i_commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;
	vkCmdSetScissor(i_commandBuffer, 0, 1, &scissor);

	vkCmdDraw(i_commandBuffer, 3, 1, 0, 0);

	//Finish render pass
	vkCmdEndRenderPass(i_commandBuffer);

	if (vkEndCommandBuffer(i_commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI