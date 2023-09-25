#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace VulkanAPI
{
class ValidationLayer;
class PhysicalDevice;
class LogicalDevice;
}

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class Instance
{

public:
	Instance(std::unique_ptr<ValidationLayer>& i_validationLayer);
	~Instance();

	VkInstance GetInstance();

	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();

private:
	std::vector<const char*> GetRequiredExtensions();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& i_createInfo);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT i_messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT i_messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* i_callbackData,
		void* i_userData);

private:
	VkInstance m_instance;

	std::unique_ptr<ValidationLayer>& m_validationLayer;

	std::unique_ptr<PhysicalDevice> m_physicalDevice;
	std::unique_ptr<LogicalDevice> m_logicalDevice;

	VkDebugUtilsMessengerEXT m_debugMessenger;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI