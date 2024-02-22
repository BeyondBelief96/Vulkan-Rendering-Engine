#ifndef TREK_MODEL_H
#define TREK_MODEL_H
#include "trek_core.h"
#include "trek_swapchain.h"
#include "trek_buffer.h"
//libs
#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

//std
#include <memory>


namespace Trek
{
    class TrekModel
    {
    public:
        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec3 normal{};
            glm::vec2 uv{};
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex& other) const
            {
                return
            	pos == other.pos && 
                    color == other.color && 
                    normal == other.normal &&
                    uv == other.uv;
            }
        };

        struct Data
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string& filePath);
        };

        TrekModel(
            TrekCore& trekDevice,
            const TrekModel::Data& data);
        ~TrekModel();

        TrekModel(const TrekModel&) = delete;
        TrekModel& operator=(const TrekModel&) = delete;
        TrekModel(const TrekModel&&) = delete;
        TrekModel& operator=(const TrekModel&&) = delete;

        static std::unique_ptr<TrekModel> createModelFromFile(
            TrekCore& device,
            const std::string& filePath);

        void bind(VkCommandBuffer commandBuffer) const;
        void draw(VkCommandBuffer commandBuffer) const;

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffer(const std::vector<uint32_t>& indices);

        TrekCore& trekDevice;
        
        std::unique_ptr<TrekBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<TrekBuffer> indexBuffer;
        uint32_t indexCount;
        
    };
}


#endif