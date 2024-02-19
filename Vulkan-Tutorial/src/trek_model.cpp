#include "trek_model.h"

namespace Trek
{
	std::vector<VkVertexInputBindingDescription> TrekModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> TrekModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}

	TrekModel::TrekModel(TrekCore& trekDevice, const std::vector<Vertex>& vertices)
		: trekDevice{trekDevice}
	{
		createVertexBuffers(vertices);
	}

	TrekModel::~TrekModel()
	{
		vkDestroyBuffer(trekDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(trekDevice.device(), vertexBufferMemory, nullptr);
	}

	void TrekModel::bind(const VkCommandBuffer commandBuffer) const
	{
		const VkBuffer buffers[] = { vertexBuffer };
		const VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void TrekModel::draw(const VkCommandBuffer commandBuffer) const
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void TrekModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be atleast 3");
		const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		trekDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);

		void* data;
		vkMapMemory(trekDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(trekDevice.device(), vertexBufferMemory);
	}
}
