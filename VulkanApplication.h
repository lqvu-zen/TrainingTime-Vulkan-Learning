#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanApplication
{
public:
	VulkanApplication();
	~VulkanApplication();

	void Run();
private:
	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void CleanUp();

	const uint32_t k_glfwWindowWidth = 800;
	const uint32_t k_glfwWindowHeight = 600;
	const std::string k_glfwWindowTitle = "Vulkan";

	GLFWwindow* m_glfwWindow;
};
