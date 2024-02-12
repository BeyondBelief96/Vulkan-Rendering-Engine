#ifndef GLFW_CORE_H
#define GLFW_CORE_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "validation_layers.h"
#include "window_core.h"

namespace Trek
{
    class GLFWCore : public WindowCore

    {
    public:
        GLFWCore(const int windowWidth, const int windowHeight, const char* windowTitle);
        virtual ~GLFWCore() override;
        GLFWCore(const GLFWCore& core) = delete;
        GLFWCore& operator=(const GLFWCore&) = delete;
        GLFWwindow* GetWindow() const { return m_window; }
        int GetWindowWidth() const { return m_windowWidth; }
        int GetWindowHeight() const { return m_windowHeight; }
        VkExtent2D GetExtent() const {
            return
            { static_cast<uint32_t>(m_windowWidth),
                static_cast<uint32_t>(m_windowHeight) };
        }

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
    private:
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

        int m_windowWidth;
        int m_windowHeight;
        GLFWwindow* m_window;
        bool m_framebufferResized = false;
    };
}

#endif
