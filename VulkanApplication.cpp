#include "VulkanApplication.h"

VulkanApplication::VulkanApplication()
{
}

VulkanApplication::~VulkanApplication()
{
}

void VulkanApplication::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	CleanUp();
}

void VulkanApplication::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_glfwWindow = glfwCreateWindow(k_glfwWindowWidth, k_glfwWindowHeight, k_glfwWindowTitle.c_str(), nullptr, nullptr);
}

void VulkanApplication::InitVulkan()
{
}

void VulkanApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_glfwWindow))
	{
		glfwPollEvents();
	}
}

void VulkanApplication::CleanUp()
{
	glfwDestroyWindow(m_glfwWindow);

	glfwTerminate();
}
