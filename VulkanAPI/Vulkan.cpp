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

void Vulkan::Init(std::unique_ptr<Window>& i_window, std::unique_ptr<FileSystem>& i_fileSystem)
{
	//Add validation layers
	m_validationLayer->AddLayer("VK_LAYER_KHRONOS_validation");

	//Init vulkan
	m_instance = std::make_unique<Instance>(m_validationLayer, i_window, i_fileSystem);
	m_instance->SetupDebugMessenger();
	m_instance->CreateSurface();
	m_instance->PickPhysicalDevice();
	m_instance->CreateLogicalDevice();
	m_instance->CreateSwapChain();
	m_instance->CreateImageViews();
	m_instance->CreateRenderPass();
	m_instance->CreateGraphicsPipeline();
	m_instance->CreateFramebuffers();
	m_instance->CreateCommandPool();
	m_instance->CreateVertexBuffer();
	m_instance->CreateCommandBuffers();
	m_instance->CreateSyncObjects();
}

///////////////////////////////////////////////////////////////////////////////

void Vulkan::Shutdown()
{
	m_instance->Shutdown();
}

///////////////////////////////////////////////////////////////////////////////

void Vulkan::Update()
{
	m_instance->DrawFrame();
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI