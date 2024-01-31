#ifndef GLFW_CORE_H
#define GLFW_CORE_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "validation_layers.h"
#include "window_core.h"

class GLFWCore : public WindowCore

{
public:
    GLFWCore() = default;
    GLFWwindow* GetWindow() const { return m_window; }
    int GetWindowWidth() const { return m_windowWidth; }
    int GetWindowHeight() const { return m_windowHeight; }

    void Init(int windowWidth, int windowHeight, const char* windowTitle);
    void Cleanup() const;
    bool HasFrameBufferResized() const { return m_framebufferResized; } 
    void SetFrameBufferResized(const bool frameBufferResized) { m_framebufferResized = frameBufferResized; }
    virtual VkSurfaceKHR CreateVulkanSurface(const VkInstance& instance) const override;

	static std::vector<const char*> GetRequiredGlfwExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	    const auto glfwCore = reinterpret_cast<GLFWCore*>(glfwGetWindowUserPointer(window));
        glfwCore->m_framebufferResized = true;
        glfwGetWindowSize(window, &glfwCore->m_windowWidth, &glfwCore->m_windowHeight);
    }

private:
    int m_windowWidth;
    int m_windowHeight;
    GLFWwindow* m_window;
    bool m_framebufferResized = false;
};
#endif
