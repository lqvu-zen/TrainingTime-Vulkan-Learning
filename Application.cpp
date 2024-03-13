#include "Application.h"

#include "Window.h"
#include "FileSystem.h"

#include "VulkanAPI/Vulkan.h"

Application::Application()
	: m_vulkan(nullptr)
{
	m_window = std::make_unique<Window>();
	m_fileSystem = std::make_unique<FileSystem>();
}

Application::~Application()
{
}

void Application::Run()
{
	InitVulkan();
	MainLoop();
	CleanUp();
}

void Application::InitVulkan()
{
	m_vulkan = std::make_unique<VulkanAPI::Vulkan>();

	m_vulkan->CreateInstance(m_window, m_fileSystem);
	m_vulkan->SetupDebugMessenger();
	m_vulkan->CreateSurface();
	m_vulkan->PickPhysicalDevice();
	m_vulkan->CreateLogicalDevice();
	m_vulkan->CreateSwapChain();
	m_vulkan->CreateImageViews();
	m_vulkan->CreateRenderPass();
	m_vulkan->CreateGraphicsPipeline();
	m_vulkan->CreateFramebuffers();
	m_vulkan->CreateCommandPool();
	m_vulkan->CreateCommandBuffers();
	m_vulkan->CreateSyncObjects();
}

void Application::MainLoop()
{
	while (!m_window->IsClosing())
	{
		m_window->Update();
		m_vulkan->DrawFrame();
	}

	m_vulkan->Shutdown();
}

void Application::CleanUp()
{
}



