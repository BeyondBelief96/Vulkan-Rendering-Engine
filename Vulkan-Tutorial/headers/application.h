#ifndef APPLICATION_H
#define APPLICATION_H
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <array>

#include "glfw_core.h"
#include "vulkan_core.h"

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription GetBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }
};

const std::vector<Vertex> VERTICES = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> INDICES = {
    0, 1, 2, 2, 3, 0
};

class Application
{
public:
    void Run();
private:
    void DrawFrame();

    void InitVulkan();

    void MainLoop();

    void Cleanup() const;

    void CreateLogicalDevice();

    void CreateSwapChain();

    void RecreateSwapChain(const VkDevice& device);

    void CleanupSwapChain() const;

    void CreateImageViews();

    void CreateRenderPass();

    void CreateGraphicsPipeline();

    void CreateFrameBuffers();

    void CreateCommandPool();

    void CreateVertexBuffer();

    void CreateIndexBuffer();

    void CreateCommandBuffers();

    void CreateSyncObjects();

    void RecordCommandBuffer(uint32_t imageIndex) const;

    VulkanCore m_core;
    GLFWCore m_glfw;
    VkDevice m_device = nullptr;
    VkQueue m_graphicsQueue = nullptr;
    VkQueue m_presentQueue = nullptr;
    VkQueue m_transferQueue = nullptr;
    VkSwapchainKHR m_swapChain = nullptr;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat{};
    VkExtent2D m_swapChainExtent{};
    std::vector<VkImageView> m_swapChainImageViews;
    VkRenderPass m_renderPass = nullptr;
    VkPipelineLayout m_pipelineLayout = nullptr;
    VkPipeline m_graphicsPipeline = nullptr;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkCommandPool m_graphicsCommandPool = nullptr;
    VkCommandPool m_transferCommandPool = nullptr;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    VkBuffer m_vertexBuffer = nullptr;
    VkDeviceMemory m_vertexBufferMemory = nullptr;
    VkBuffer m_indexBuffer = nullptr;
    VkDeviceMemory m_indexBufferMemory = nullptr;


	int m_max_Frames_In_Flight = 2;
    uint32_t m_current_Frame = 0;
};


#endif
