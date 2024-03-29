// ReSharper disable CppUseStructuredBinding
#include <iostream>
#include <cstdlib>

#include "application.h"

int main() {
	try
	{
		Trek::Application app{};
        app.run();
    } catch(const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
