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
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(k_glfwWindowWidth, k_glfwWindowHeight, k_glfwWindowTitle.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);
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

bool Window::IsMinimizing()
{
	WindowSize currentWindowSize = GetWindowSize();

	return (currentWindowSize.width == 0) || (currentWindowSize.height == 0) ;
}

GLFWwindow* Window::GetWindow()
{
	return m_window;
}

WindowSize Window::GetWindowSize()
{
	WindowSize currentWindowSize;
	currentWindowSize.width = 0;
	currentWindowSize.height = 0;

	glfwGetFramebufferSize(m_window, &currentWindowSize.width, &currentWindowSize.height);

	return currentWindowSize;
}

bool Window::IsFramebufferResized()
{
	return m_framebufferResized;
}

void Window::SetFramebufferResized(bool i_resized)
{
	if (i_resized != m_framebufferResized)
	{
		m_framebufferResized = i_resized;
	}
}

void Window::FramebufferResizeCallback(GLFWwindow* i_window, int i_width, int i_height)
{
	auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(i_window));
	window->SetFramebufferResized(true);
}
