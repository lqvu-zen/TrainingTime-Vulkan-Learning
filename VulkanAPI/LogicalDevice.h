#pragma once

#include <vulkan/vulkan.h>

#include <memory>

namespace VulkanAPI
{
class ValidationLayer;
class PhysicalDevice;
}

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class LogicalDevice
{
public:
	LogicalDevice(std::unique_ptr<ValidationLayer>& i_validationLayer, std::unique_ptr<PhysicalDevice>& i_physicalDevice);
	~LogicalDevice();

	VkDevice GetDevice();
	void CleanUp();

private:
	VkDevice m_device;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI