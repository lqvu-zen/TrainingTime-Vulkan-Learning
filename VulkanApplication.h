#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <optional>

class VulkanApplication
{
public:
	VulkanApplication();
	~VulkanApplication();

	void Run();
private:
	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void CleanUp();

	void CreateInstance();
	bool CheckValidationLayerSupport();
	void SetupDebugMessenger();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& i_createInfo);
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	bool IsDeviceSuitable(VkPhysicalDevice device);
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		bool IsComplete()
		{
			return graphicsFamily.has_value();
		}
	};
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	std::vector<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT i_messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT i_messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* i_callbackData,
		void* i_userData);

private:
	const uint32_t k_glfwWindowWidth = 800;
	const uint32_t k_glfwWindowHeight = 600;
	const std::string k_glfwWindowTitle = "Vulkan";

	const std::vector<const char*> m_validationLayers = 
	{
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool k_enableValidationLayers = false;
#else
	const bool k_enableValidationLayers = true;
#endif

	GLFWwindow* m_glfwWindow;

	VkInstance m_vkInstance;

	VkDebugUtilsMessengerEXT m_debugMessenger;
};
