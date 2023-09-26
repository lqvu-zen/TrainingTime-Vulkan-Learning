#include "Window.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

Window::Window()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(k_glfwWindowWidth, k_glfwWindowHeight, k_glfwWindowTitle.c_str(), nullptr, nullptr);
}

Window::~Window()
{
	glfwDestroyWindow(m_window);

	glfwTerminate();
}

void Window::Update()
{
	glfwPollEvents();
}

bool Window::IsClosing()
{
	return glfwWindowShouldClose(m_window);
}

GLFWwindow* Window::GetWindow()
{
	return m_window;
}
