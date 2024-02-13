#include "trek_window.h"

// std
#include <stdexcept>

namespace Trek
{
	TrekWindow::TrekWindow(const int width, const int height, std::string name)
		: width{ width }, height{ height }, windowName{std::move(name)}
	{
		initWindow();
	}

	TrekWindow::~TrekWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void TrekWindow::createWindowSurface(const VkInstance instance, VkSurfaceKHR* surface) const
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface.");
		}
	}

	void TrekWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}
}
