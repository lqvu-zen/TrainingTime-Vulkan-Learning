#pragma once

#include <vulkan/vulkan.h>

#include <memory>

namespace VulkanAPI
{
class Instance;
struct QueueFamilyIndices;
}

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class PhysicalDevice
{
public:
	PhysicalDevice(VkInstance i_instance);
	~PhysicalDevice();

	VkPhysicalDevice GetDevice();
	QueueFamilyIndices GetQueueFamilies();

private:
	bool IsDeviceSuitable(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

private:
	VkPhysicalDevice m_device = VK_NULL_HANDLE;

};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI
