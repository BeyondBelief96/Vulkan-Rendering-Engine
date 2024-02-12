#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "glfw_core.h"

#include <stdexcept>

namespace Trek
{
	GLFWCore::GLFWCore(const int windowWidth, const int windowHeight, const char* windowTitle)
	{
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);

        m_windowWidth = windowWidth;
        m_windowHeight = windowHeight;
	}

    GLFWCore::~GLFWCore()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    VkSurfaceKHR GLFWCore::CreateVulkanSurface(const VkInstance& instance) const
    {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface.");
        }

        return surface;
    }
	void GLFWCore::FramebufferResizeCallback(GLFWwindow* window, const int width, const int height)
    {
        const auto glfwCore = reinterpret_cast<GLFWCore*>(glfwGetWindowUserPointer(window));
        glfwCore->m_framebufferResized = true;
        glfwCore->m_windowWidth = width;
        glfwCore->m_windowHeight = height;
    }
}


