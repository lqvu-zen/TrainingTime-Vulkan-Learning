#include "PhysicalDevice.h"

#include <stdexcept>
#include <vector>

#include "QueueFamilyIndices.h"

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

VulkanAPI::PhysicalDevice::PhysicalDevice(VkInstance i_instance, VkSurfaceKHR i_surface)
	: m_device(nullptr)
	, m_surface(i_surface)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(i_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(i_instance, &deviceCount, devices.data());

	for (const VkPhysicalDevice& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_device = device;
			break;
		}
	}

	if (m_device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

///////////////////////////////////////////////////////////////////////////////

PhysicalDevice::~PhysicalDevice()
{
}

VkPhysicalDevice PhysicalDevice::GetDevice()
{
	return m_device;
}

QueueFamilyIndices PhysicalDevice::GetQueueFamilies()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_device);

	return indices;
}

bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	return indices.IsComplete();
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int index = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = index;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, index, m_surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = index;
		}

		if (indices.IsComplete())
		{
			break;
		}

		++index;
	}

	return indices;
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI
