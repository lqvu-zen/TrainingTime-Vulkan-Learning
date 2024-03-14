#pragma once

#include <string>

struct GLFWwindow;

struct WindowSize
{
	int height;
	int width;
};

class Window
{
public:
	Window();
	~Window();

	void Update();
	bool IsClosing();
	bool IsMinimizing();
	GLFWwindow* GetWindow();
	WindowSize GetWindowSize();

	bool IsFramebufferResized();
	void SetFramebufferResized(bool i_resized);

	static void FramebufferResizeCallback(GLFWwindow* i_window, int i_width, int i_height);

private:
	GLFWwindow* m_window;

	const uint32_t k_glfwWindowWidth = 800;
	const uint32_t k_glfwWindowHeight = 600;
	const std::string k_glfwWindowTitle = "Vulkan";

	bool m_framebufferResized = false;
};

