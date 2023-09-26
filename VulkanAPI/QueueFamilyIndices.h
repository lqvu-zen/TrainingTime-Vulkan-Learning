#pragma once

#include <optional>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI