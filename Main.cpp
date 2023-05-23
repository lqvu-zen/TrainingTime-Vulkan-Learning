#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "VulkanApplication.h"

int main()
{
	VulkanApplication vulkanApplication;

	try
	{
		vulkanApplication.Run();
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}