#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
	Window();
	~Window();

	void Update();
	bool IsClosing();
	GLFWwindow* GetWindow();

private:
	GLFWwindow* m_window;

	const uint32_t k_glfwWindowWidth = 800;
	const uint32_t k_glfwWindowHeight = 600;
	const std::string k_glfwWindowTitle = "Vulkan";
};

