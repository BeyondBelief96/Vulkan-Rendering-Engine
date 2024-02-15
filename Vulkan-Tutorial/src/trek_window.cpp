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

	void TrekWindow::framebufferResizedCallback(GLFWwindow* window, const int width, const int height)
	{
		const auto trekWindow = reinterpret_cast<TrekWindow*>(glfwGetWindowUserPointer(window));
		trekWindow->frameBufferResized = true;
		trekWindow->width = width;
		trekWindow->height = height;
	}

	void TrekWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
	}
}
