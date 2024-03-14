#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class WindowSurface
{
public:
	WindowSurface(VkInstance i_instance, GLFWwindow* i_window);
	~WindowSurface();

	VkSurfaceKHR GetSurface();
	void Cleanup();

private:
	VkSurfaceKHR m_surface;

	VkInstance m_instance;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI
