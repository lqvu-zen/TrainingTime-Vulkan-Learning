#include "LogicalDevice.h"

#include <stdexcept>

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

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

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
}

LogicalDevice::~LogicalDevice()
{
}

///////////////////////////////////////////////////////////////////////////////

void LogicalDevice::CleanUp()
{
	vkDestroyDevice(m_device, nullptr);
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI