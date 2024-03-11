#include "Vulkan.h"

#include "Instance.h"
#include "ValidationLayer.h"

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

Vulkan::Vulkan()
{
	m_validationLayer = std::make_unique<ValidationLayer>();
}

///////////////////////////////////////////////////////////////////////////////

Vulkan::~Vulkan()
{
}

///////////////////////////////////////////////////////////////////////////////

void Vulkan::CreateInstance(std::unique_ptr<Window>& i_window, std::unique_ptr<FileSystem>& i_fileSystem)
{
	//Add validation layers
	m_validationLayer->AddLayer("VK_LAYER_KHRONOS_validation");

	m_instance = std::make_unique<Instance>(m_validationLayer, i_window, i_fileSystem);
}

///////////////////////////////////////////////////////////////////////////////

void Vulkan::SetupDebugMessenger()
{
	m_instance->SetupDebugMessenger();
}

///////////////////////////////////////////////////////////////////////////////

void Vulkan::CreateSurface()
{
	m_instance->CreateSurface();
}

///////////////////////////////////////////////////////////////////////////////

void Vulkan::PickPhysicalDevice()
{
	m_instance->PickPhysicalDevice();
}

void Vulkan::CreateLogicalDevice()
{
	m_instance->CreateLogicalDevice();
}

void Vulkan::CreateSwapChain()
{
	m_instance->CreateSwapChain();
}

void Vulkan::CreateImageViews()
{
	m_instance->CreateImageViews();
}

void Vulkan::CreateRenderPass()
{
	m_instance->CreateRenderPass();
}

void Vulkan::CreateGraphicsPipeline()
{
	m_instance->CreateGraphicsPipeline();
}

void Vulkan::CreateFramebuffers()
{
	m_instance->CreateFramebuffers();
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI