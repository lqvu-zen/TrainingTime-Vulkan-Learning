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

void Vulkan::CreateCommandPool()
{
	m_instance->CreateCommandPool();
}

void Vulkan::CreateCommandBuffer()
{
	m_instance->CreateCommandBuffer();
}

void Vulkan::DrawFrame()
{
	m_instance->DrawFrame();
}

void Vulkan::CreateSyncObjects()
{
	m_instance->CreateSyncObjects();
}

void Vulkan::Shutdown()
{
	m_instance->Shutdown();
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI