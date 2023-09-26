#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

struct GLFWwindow;

namespace VulkanAPI
{
class ValidationLayer;
class PhysicalDevice;
class LogicalDevice;
class WindowSurface;
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
	void CreateSurface(GLFWwindow* i_window);
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
	std::unique_ptr<WindowSurface> m_windowSurface;

	VkDebugUtilsMessengerEXT m_debugMessenger;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI