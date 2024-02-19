#ifndef TREK_MODEL_H
#define TREK_MODEL_H
#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "trek_core.h"
#include "trek_swapchain.h"

namespace Trek
{
    class TrekModel
    {
    public:
        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Data
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        TrekModel(
            TrekCore& trekDevice,
            const std::vector<Vertex>& vertices);
        ~TrekModel();

        TrekModel(const TrekModel&) = delete;
        TrekModel& operator=(const TrekModel&) = delete;
        TrekModel(const TrekModel&&) = delete;
        TrekModel& operator=(const TrekModel&&) = delete;

        void bind(VkCommandBuffer commandBuffer) const;
        void draw(VkCommandBuffer commandBuffer) const;

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffer(const std::vector<uint32_t>& indices);

        TrekCore& trekDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
        
    };
}


#endif