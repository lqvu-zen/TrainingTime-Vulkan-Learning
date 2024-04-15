#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace VulkanAPI
{
class ValidationLayer;
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
}

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class Device
{
public:
	Device(VkInstance i_instance, VkSurfaceKHR i_surface, std::unique_ptr<ValidationLayer>& i_validationLayer);
	~Device();

	void Init();
	void Cleanup();

	//Physical device
	VkPhysicalDevice GetPhysicalDevice();
	QueueFamilyIndices GetQueueFamilies();
	std::vector<const char*> GetDeviceExtensions();
	SwapChainSupportDetails GetSwapChainSupport();

	//Logical device
	VkDevice GetDevice();
	VkQueue GetGraphicsQueue();
	VkQueue GetPresentQueue();

private:
	bool IsDeviceSuitable(VkPhysicalDevice i_device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice i_device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice i_device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice i_device);

private:
	//Physical device
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	VkInstance m_instance;
	VkSurfaceKHR m_surface;
	std::unique_ptr<ValidationLayer>& m_validationLayer;

	const std::vector<const char*> m_deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	//Logical device
	VkDevice m_device;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI