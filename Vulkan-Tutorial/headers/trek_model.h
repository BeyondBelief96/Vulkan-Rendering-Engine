#ifndef TREK_MODEL_H
#define TREK_MODEL_H
#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <array>

#include "trek_core.h"
#include "trek_swap_chain.h"
#include "trek_buffer.h"

namespace Trek
{
	class TrekModel
	{
	public:
        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 color;

            static VkVertexInputBindingDescription GetBindingDescription() {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
            {
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

        struct Data
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

		TrekModel(
            TrekCore& coreDevice,
            const TrekModel::Data& data);
		~TrekModel();

		TrekModel(const TrekModel&) = delete;
		TrekModel& operator=(const TrekModel&) = delete;

		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;

	private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);

		TrekCore& m_coreDevice;

        std::unique_ptr<TrekBuffer> m_vertexBuffer;
		uint32_t m_vertexCount;

        bool m_hasIndexBuffer = false;
        std::unique_ptr<TrekBuffer> m_indexBuffer;
        uint32_t m_indexCount;
	};
}


#endif
