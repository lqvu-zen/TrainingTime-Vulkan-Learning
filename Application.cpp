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

	m_vulkan->CreateInstance();
	m_vulkan->SetupDebugMessenger();
	m_vulkan->CreateSurface(m_window->GetWindow());
	m_vulkan->PickPhysicalDevice();
	m_vulkan->CreateLogicalDevice();
}

void Application::MainLoop()
{
	while (!m_window->IsClosing())
	{
		m_window->Update();
	}
}

void Application::CleanUp()
{
}



