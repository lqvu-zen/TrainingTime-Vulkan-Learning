#include "LogicalDevice.h"

#include <stdexcept>
#include <set>

#include "QueueFamilyIndices.h"
#include "PhysicalDevice.h"
#include "ValidationLayer.h"

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

LogicalDevice::LogicalDevice(std::unique_ptr<ValidationLayer>& i_validationLayer, std::unique_ptr<PhysicalDevice>& i_physicalDevice)
	: m_device(nullptr)
	, m_graphicsQueue(nullptr)
{
	QueueFamilyIndices indices = i_physicalDevice->GetQueueFamilies();

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

	const std::vector<const char*> deviceExtensions = i_physicalDevice->GetDeviceExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	const std::vector<const char*> validationLayers = i_validationLayer->GetLayers();

	if (i_validationLayer->IsEnable())
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(i_physicalDevice->GetDevice(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create a logic device!");
	}

	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

LogicalDevice::~LogicalDevice()
{
}

///////////////////////////////////////////////////////////////////////////////

VkDevice LogicalDevice::GetDevice()
{
	return m_device;
}

///////////////////////////////////////////////////////////////////////////////

void LogicalDevice::CleanUp()
{
	vkDestroyDevice(m_device, nullptr);
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI