#pragma once

#include <memory>;

namespace VulkanAPI
{
class Instance;
class ValidationLayer;
}

class Window;

namespace VulkanAPI
{

///////////////////////////////////////////////////////////////////////////////

class Vulkan
{

public:
	Vulkan();
	~Vulkan();

	void CreateInstance(std::unique_ptr<Window>& i_window);
	void SetupDebugMessenger();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();

private:
	std::unique_ptr<Instance> m_instance;
	std::unique_ptr<ValidationLayer> m_validationLayer;

};

///////////////////////////////////////////////////////////////////////////////

}// VulkanAPI


