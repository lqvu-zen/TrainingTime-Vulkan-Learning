#pragma once

#include <memory>;

namespace VulkanAPI
{
class Instance;
class ValidationLayer;
}

class Window;
class FileSystem;

namespace VulkanAPI
{

///////////////////////////////////////////////////////////////////////////////

class Vulkan
{

public:
	Vulkan();
	~Vulkan();

	void CreateInstance(std::unique_ptr<Window>& i_window, std::unique_ptr<FileSystem>& i_fileSystem);
	void SetupDebugMessenger();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateCommandBuffer();

private:
	std::unique_ptr<Instance> m_instance;
	std::unique_ptr<ValidationLayer> m_validationLayer;

};

///////////////////////////////////////////////////////////////////////////////

}// VulkanAPI


