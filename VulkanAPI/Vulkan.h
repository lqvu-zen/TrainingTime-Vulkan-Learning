#pragma once

#include <memory>;

namespace VulkanAPI
{
class Instance;
class ValidationLayer;
}

class Window;
class FileSystem;

namespace VulkanAPI
{

///////////////////////////////////////////////////////////////////////////////

class Vulkan
{

public:
	Vulkan();
	~Vulkan();

	void Init(std::unique_ptr<Window>& i_window, std::unique_ptr<FileSystem>& i_fileSystem);
	void Shutdown();
	void Update();

private:
	std::unique_ptr<Instance> m_instance;
	std::unique_ptr<ValidationLayer> m_validationLayer;

};

///////////////////////////////////////////////////////////////////////////////

}// VulkanAPI


