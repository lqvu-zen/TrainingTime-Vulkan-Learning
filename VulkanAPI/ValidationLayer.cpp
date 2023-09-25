#include "ValidationLayer.h"

#include <vulkan/vulkan.h>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

VulkanAPI::ValidationLayer::ValidationLayer()
{
}

///////////////////////////////////////////////////////////////////////////////

VulkanAPI::ValidationLayer::~ValidationLayer()
{
}

///////////////////////////////////////////////////////////////////////////////

bool VulkanAPI::ValidationLayer::IsEnable()
{
#ifdef NDEBUG
	return false;
#else
	return true;
#endif
}

bool VulkanAPI::ValidationLayer::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////

void ValidationLayer::AddLayer(const char* i_layer)
{
	m_validationLayers.push_back(i_layer);
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<const char*> ValidationLayer::GetLayers()
{
	return m_validationLayers;
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI