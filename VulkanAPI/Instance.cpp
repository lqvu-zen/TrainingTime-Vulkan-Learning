#include "Instance.h"

#include <iostream>
#include <stdexcept>

#include "ValidationLayer.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WindowSurface.h"

//Extensions
VkResult CreateDebugUtilsMessengerEXT(VkInstance i_instance, const VkDebugUtilsMessengerCreateInfoEXT* i_createInfo, const VkAllocationCallbacks* i_allocator, VkDebugUtilsMessengerEXT* i_debugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(i_instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(i_instance, i_createInfo, i_allocator, i_debugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance i_instance, VkDebugUtilsMessengerEXT i_debugMessenger, const VkAllocationCallbacks* i_allocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(i_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(i_instance, i_debugMessenger, i_allocator);
	}
}
//Extensions

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

VulkanAPI::Instance::Instance(std::unique_ptr<ValidationLayer>& i_validationLayer)
	: m_instance(nullptr)
	, m_validationLayer(i_validationLayer)
{
	if (m_validationLayer->IsEnable() && !m_validationLayer->CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "My Vulkan Training";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	const std::vector<const char*> validationLayers = m_validationLayer->GetLayers();

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (m_validationLayer->IsEnable()) {

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	VkResult createInstanceResult = vkCreateInstance(&createInfo, nullptr, &m_instance);
	if (createInstanceResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

///////////////////////////////////////////////////////////////////////////////

Instance::~Instance()
{
	m_logicalDevice->CleanUp();

	if (m_validationLayer->IsEnable()) {
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}

	m_windowSurface->CleanUp();

	vkDestroyInstance(m_instance, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

VkInstance Instance::GetInstance()
{
	return m_instance;
}

///////////////////////////////////////////////////////////////////////////////

void Instance::SetupDebugMessenger()
{
	if (!m_validationLayer->IsEnable()) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateSurface(GLFWwindow* i_window)
{
	m_windowSurface = std::make_unique<WindowSurface>(m_instance, i_window);
}

///////////////////////////////////////////////////////////////////////////////

void Instance::PickPhysicalDevice()
{
	m_physicalDevice = std::make_unique<PhysicalDevice>(m_instance, m_windowSurface->GetSurface());
}

///////////////////////////////////////////////////////////////////////////////

void Instance::CreateLogicalDevice()
{
	m_logicalDevice = std::make_unique<LogicalDevice>(m_validationLayer, m_physicalDevice);
}

///////////////////////////////////////////////////////////////////////////////

std::vector<const char*> Instance::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_validationLayer->IsEnable())
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

///////////////////////////////////////////////////////////////////////////////

void Instance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& i_createInfo)
{
	i_createInfo = {};
	i_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	i_createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	i_createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	i_createInfo.pfnUserCallback = DebugCallback;
	i_createInfo.pUserData = nullptr; // Optional
}

///////////////////////////////////////////////////////////////////////////////

VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT i_messageSeverity, VkDebugUtilsMessageTypeFlagsEXT i_messageType, const VkDebugUtilsMessengerCallbackDataEXT* i_callbackData, void* i_userData)
{
	std::cerr << "validation layer: " << i_callbackData->pMessage << std::endl;

	return VK_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI