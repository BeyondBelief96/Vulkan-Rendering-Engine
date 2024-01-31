// ReSharper disable CppUseStructuredBinding
#include <iostream>
#include <cstdlib>

#include "application.h"

int main() {
	try
	{
		Application app;
		app.Run();
    } catch(const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
