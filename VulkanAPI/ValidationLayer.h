#pragma once

#include <vector>

class Window;

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

class ValidationLayer
{
public:
	ValidationLayer();
	~ValidationLayer();

	bool IsEnable();
	bool CheckValidationLayerSupport();

	void AddLayer(const char* i_layer);
	const std::vector<const char*> GetLayers();

private:
	std::vector<const char*> m_validationLayers;
};

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI