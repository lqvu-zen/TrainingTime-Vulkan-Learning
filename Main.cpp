#include <exception>
#include <iostream>

#include "Application.h"

int main()
{
	Application mainApplication;

	try
	{
		mainApplication.Run();
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}