#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace VulkanAPI
{
class Instance;
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
}

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class PhysicalDevice
{
public:
	PhysicalDevice(VkInstance i_instance, VkSurfaceKHR i_surface);
	~PhysicalDevice();

	VkPhysicalDevice GetDevice();
	QueueFamilyIndices GetQueueFamilies();
	std::vector<const char*> GetDeviceExtensions();
	SwapChainSupportDetails GetSwapChainSupport();

private:
	bool IsDeviceSuitable(VkPhysicalDevice i_device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice i_device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice i_device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice i_device);

private:
	VkPhysicalDevice m_device = VK_NULL_HANDLE;

	VkSurfaceKHR m_surface;

	const std::vector<const char*> m_deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI
