#include "Device.h"

#include <stdexcept>
#include <set>

#include "QueueFamilyIndices.h"
#include "ValidationLayer.h"
#include "SwapChainSupportDetails.h"

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

Device::Device(VkInstance i_instance, VkSurfaceKHR i_surface, std::unique_ptr<ValidationLayer>& i_validationLayer)
	: m_physicalDevice(nullptr)
	, m_device(nullptr)
	, m_graphicsQueue(nullptr)
	, m_instance(i_instance)
	, m_surface(i_surface)
	, m_validationLayer(i_validationLayer)
{
}

Device::~Device()
{
}

///////////////////////////////////////////////////////////////////////////////

void Device::Init()
{
	//Pick Physical Device
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	for (const VkPhysicalDevice& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	//Create Logical Device
	QueueFamilyIndices indices = GetQueueFamilies();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	const std::vector<const char*> deviceExtensions = GetDeviceExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	const std::vector<const char*> validationLayers = m_validationLayer->GetLayers();

	if (m_validationLayer->IsEnable())
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create a logic device!");
	}

	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

///////////////////////////////////////////////////////////////////////////////

void Device::Cleanup()
{
	vkDestroyDevice(m_device, nullptr);
}


///////////////////////////////////////////////////////////////////////////////

VkPhysicalDevice Device::GetPhysicalDevice()
{
	return m_physicalDevice;
}

///////////////////////////////////////////////////////////////////////////////

QueueFamilyIndices Device::GetQueueFamilies()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

	return indices;
}

///////////////////////////////////////////////////////////////////////////////

std::vector<const char*> Device::GetDeviceExtensions()
{
	return m_deviceExtensions;
}

///////////////////////////////////////////////////////////////////////////////

SwapChainSupportDetails Device::GetSwapChainSupport()
{
	SwapChainSupportDetails swapChainSupportDetails = QuerySwapChainSupport(m_physicalDevice);

	return swapChainSupportDetails;
}

///////////////////////////////////////////////////////////////////////////////

uint32_t Device::GetMemoryType(uint32_t i_typeFilter, VkMemoryPropertyFlags i_properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((i_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & i_properties) == i_properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

///////////////////////////////////////////////////////////////////////////////

VkDevice Device::GetDevice()
{
	return m_device;
}

///////////////////////////////////////////////////////////////////////////////

VkQueue Device::GetGraphicsQueue()
{
	return m_graphicsQueue;
}

///////////////////////////////////////////////////////////////////////////////

VkQueue Device::GetPresentQueue()
{
	return m_presentQueue;
}

///////////////////////////////////////////////////////////////////////////////

bool Device::IsDeviceSuitable(VkPhysicalDevice i_device)
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

QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice i_device)
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

bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice i_device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(i_device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(i_device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

///////////////////////////////////////////////////////////////////////////////

SwapChainSupportDetails Device::QuerySwapChainSupport(VkPhysicalDevice i_device)
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