#ifndef APPLICATION_H
#define APPLICATION_H
#include <vector>
#include <vulkan/vulkan_core.h>

#include "glfw_core.h"
#include "trek_core.h"
#include "trek_model.h"
#include "trek_pipeline.h"
#include "trek_swap_chain.h"
#include "trek_buffer.h"
#include "trek_descriptor_set.h"

namespace Trek
{
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class Application
    {
    public:
        Application();
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        void Run();
    private:
        void DrawFrame();

        void InitVulkan();

        void MainLoop();

        void Cleanup() const;

        void CreatePipelineLayout();

        void CreatePipeline();

        void RecreateSwapChain();

        void CreateDescriptorSets();

        void CreateCommandBuffers();

        void RecordCommandBuffer(uint32_t imageIndex) const;

        void UpdateUniformBuffers(uint32_t currentImage) const;

        void LoadModels();

    	std::unique_ptr<TrekModel> CreateCubeModel(const glm::vec3 offset);

        void FreeCommandBuffers();

        GLFWCore m_glfw{800, 600, "Vulkan Tutorial!"};
        TrekCore m_core{&m_glfw};
        std::unique_ptr<TrekSwapChain> m_trekSwapChain;
        std::shared_ptr<TrekModel> m_model;
        std::vector<std::unique_ptr<TrekBuffer>> m_globalUboBuffers{ TrekSwapChain::MAX_FRAMES_IN_FLIGHT };
        std::vector<VkCommandBuffer> m_commandBuffers;
        std::unique_ptr<TrekPipeline> m_trekPipeline;
        VkPipelineLayout m_pipelineLayout = nullptr;

        std::unique_ptr<TrekDescriptorSetLayout> m_globalSetLayout{};
        std::unique_ptr<TrekDescriptorPool> m_globalPool{};
        std::vector<VkDescriptorSet> m_globalDescriptorSets{};
        
    };
}

#endif
