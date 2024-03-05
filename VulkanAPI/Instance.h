#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace VulkanAPI
{
class ValidationLayer;
class PhysicalDevice;
class LogicalDevice;
class WindowSurface;
struct SwapChainSupportDetails;
}

class Window;

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class Instance
{

public:
	Instance(std::unique_ptr<ValidationLayer>& i_validationLayer, std::unique_ptr<Window>& i_window);
	~Instance();

	VkInstance GetInstance();

	void SetupDebugMessenger();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();

private:
	std::vector<const char*> GetRequiredExtensions();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& i_createInfo);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT i_messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT i_messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* i_callbackData,
		void* i_userData);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& i_availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& i_availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& i_capabilities);

private:
	VkInstance m_instance;

	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;

	std::unique_ptr<ValidationLayer>& m_validationLayer;
	std::unique_ptr<Window>& m_window;

	std::unique_ptr<PhysicalDevice> m_physicalDevice;
	std::unique_ptr<LogicalDevice> m_logicalDevice;
	std::unique_ptr<WindowSurface> m_windowSurface;

	VkDebugUtilsMessengerEXT m_debugMessenger;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI