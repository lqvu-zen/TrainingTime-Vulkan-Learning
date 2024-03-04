#include "PhysicalDevice.h"

#include <stdexcept>
#include <vector>
#include <set>

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

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

///////////////////////////////////////////////////////////////////////////////

VkPhysicalDevice PhysicalDevice::GetDevice()
{
	return m_device;
}

///////////////////////////////////////////////////////////////////////////////

QueueFamilyIndices PhysicalDevice::GetQueueFamilies()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_device);

	return indices;
}

///////////////////////////////////////////////////////////////////////////////

std::vector<const char*> PhysicalDevice::GetDeviceExtensions()
{
	return m_deviceExtensions;
}

///////////////////////////////////////////////////////////////////////////////

SwapChainSupportDetails PhysicalDevice::GetSwapChainSupport()
{
	SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(m_device);

	return swapChainSupportDetails;
}

///////////////////////////////////////////////////////////////////////////////

bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice i_device)
{
	QueueFamilyIndices indices = FindQueueFamilies(i_device);

	bool extensionsSupported = CheckDeviceExtensionSupport(i_device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(i_device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

///////////////////////////////////////////////////////////////////////////////

QueueFamilyIndices PhysicalDevice::FindQueueFamilies(VkPhysicalDevice i_device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(i_device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(i_device, &queueFamilyCount, queueFamilies.data());

	int index = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = index;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(i_device, index, m_surface, &presentSupport);

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

bool PhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice i_device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(i_device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(i_device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

///////////////////////////////////////////////////////////////////////////////

SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice i_device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(i_device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(i_device, m_surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(i_device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(i_device, m_surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(i_device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI
