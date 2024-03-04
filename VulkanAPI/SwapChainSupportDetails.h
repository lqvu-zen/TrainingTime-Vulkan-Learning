#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

namespace VulkanAPI
{
	///////////////////////////////////////////////////////////////////////////////

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI