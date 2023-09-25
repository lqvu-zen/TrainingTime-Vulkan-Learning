#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Window::Window()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_glfwWindow = glfwCreateWindow(k_glfwWindowWidth, k_glfwWindowHeight, k_glfwWindowTitle.c_str(), nullptr, nullptr);
}

Window::~Window()
{
	glfwDestroyWindow(m_glfwWindow);

	glfwTerminate();
}

void Window::Update()
{
	glfwPollEvents();
}

bool Window::IsClosing()
{
	return glfwWindowShouldClose(m_glfwWindow);
}
