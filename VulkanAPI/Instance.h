#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <memory>
#include <vector>

namespace VulkanAPI
{
class ValidationLayer;
class Device;
class WindowSurface;
class RenderPass;
class Buffer;
class CommandBuffer;
struct SwapChainSupportDetails;
}

class Window;
class FileSystem;

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class Instance
{

public:
	Instance(std::unique_ptr<Window>& i_window, std::unique_ptr<FileSystem>& i_fileSystem);
	~Instance();

	VkInstance GetInstance();

	void SetupDebugMessenger();
	void CreateSurface();
	void CreateDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateBuffer();
	void CreateCommandBuffers();
	void DrawFrame();
	void CreateSyncObjects();

	void CleanupSwapChain();
	void RecreateSwapChain();

	void Cleanup();
	void Shutdown();

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

	VkShaderModule CreateShaderModule(const std::vector<char>& i_code);

	void RecordCommandBuffer(VkCommandBuffer i_commandBuffer, uint32_t i_imageIndex);

private:
	const int K_MAX_FRAMES_IN_FLIGHT = 2;

private:
	VkDebugUtilsMessengerEXT m_debugMessenger;

	std::unique_ptr<ValidationLayer> m_validationLayer;
	VkInstance m_instance;

	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_graphicsPipeline;

	std::unique_ptr<Window>& m_window;
	std::unique_ptr<FileSystem>& m_fileSystem;

	std::unique_ptr<Device> m_device;
	std::unique_ptr<WindowSurface> m_windowSurface;
	std::unique_ptr<RenderPass> m_renderPass;

	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;

	VkCommandPool m_commandPool;

	std::unique_ptr<Buffer> m_buffer;
	std::unique_ptr<CommandBuffer> m_commandBuffer;

	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;

	uint32_t m_currentFrame = 0;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI