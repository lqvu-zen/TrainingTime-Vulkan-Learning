#pragma once

#include <memory>

namespace VulkanAPI
{
class Instance;
class ValidationLayer;
}

namespace VulkanAPI
{

///////////////////////////////////////////////////////////////////////////////

class Vulkan
{

public:
	Vulkan();
	~Vulkan();

	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();

private:
	std::unique_ptr<Instance> m_instance;
	std::unique_ptr<ValidationLayer> m_validationLayer;

};

///////////////////////////////////////////////////////////////////////////////

}// VulkanAPI


