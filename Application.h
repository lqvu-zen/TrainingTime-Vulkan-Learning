#pragma once

#include <memory>

class Window;
class FileSystem;

namespace VulkanAPI
{
class Vulkan;
}// Vulkan

class Application
{
public:
	Application();
	~Application();

	void Run();

private:
	void InitVulkan();
	void MainLoop();
	void CleanUp();

private:
	std::unique_ptr<VulkanAPI::Vulkan> m_vulkan;

	std::unique_ptr<Window> m_window;
	std::unique_ptr<FileSystem> m_fileSystem;
};
