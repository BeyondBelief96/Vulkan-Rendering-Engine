#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "physical_device.h"

class Scene
{
public:
	virtual ~Scene() = default;
	virtual void InitWindow() = 0;
    virtual void InitVulkan() = 0;
    virtual void MainLoop() = 0;
    virtual void Cleanup() = 0;
protected:
    virtual void DrawFrame() = 0;
    virtual void CreateInstance() = 0;
    virtual void SetupDebugMessenger() = 0;
    virtual void CreateSurface() = 0;
    virtual void PickPhysicalDevice() = 0;
    virtual void CreateLogicalDevice() = 0;
    virtual void CreateSwapChain() = 0;
    virtual void CreateImageViews() = 0;
    virtual void CreateRenderPass() = 0;
    virtual void CreateGraphicsPipeline() = 0;
    virtual void CreateFrameBuffers() = 0;
    virtual void CreateCommandPool() = 0;
    virtual void CreateCommandBuffers() = 0;
    virtual void CreateSyncObjects() = 0;

    virtual void RecreateSwapChain(const VkDevice& device) = 0;
    virtual void CleanupSwapChain() const = 0;
    virtual bool IsDeviceSuitable(const VkPhysicalDevice device) = 0;

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	    const auto scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
        scene->m_framebufferResized = true;
    }

    const uint32_t m_width = 800;
    const uint32_t m_height = 600;
    const int m_max_Frames_In_Flight = 2;
    uint32_t m_current_Frame = 0;

    GLFWwindow* m_window = nullptr;
    VkInstance m_instance = nullptr;
    VkSurfaceKHR m_surface = nullptr;
    VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    Dnq::QueueFamilyIndices m_indices;
    VkDevice m_device = nullptr;
    VkQueue m_graphicsQueue = nullptr;
    VkQueue m_presentQueue = nullptr;
    VkSwapchainKHR m_swapChain = nullptr;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat{};
    VkExtent2D m_swapChainExtent{};
    std::vector<VkImageView> m_swapChainImageViews;
    VkRenderPass m_renderPass = nullptr;
    VkPipelineLayout m_pipelineLayout = nullptr;
    VkPipeline m_graphicsPipeline = nullptr;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkCommandPool m_commandPool = nullptr;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    bool m_framebufferResized = false;
};

#endif
