#include "trek_model.h"
#include "trek_utils.h"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

//std
#include <unordered_map>

namespace std
{
	template<>
	struct hash<Trek::TrekModel::Vertex>
	{
		size_t operator()(Trek::TrekModel::Vertex const& vertex) const
		{
			size_t seed = 0;
			Trek::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

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

	TrekModel::TrekModel(TrekCore& trekDevice, const TrekModel::Data& data)
		: trekDevice{trekDevice}
	{
		createVertexBuffers(data.vertices);
		createIndexBuffer(data.indices);
	}

	TrekModel::~TrekModel()
	{
		vkDestroyBuffer(trekDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(trekDevice.device(), vertexBufferMemory, nullptr);

		if(hasIndexBuffer)
		{
			vkDestroyBuffer(trekDevice.device(), indexBuffer, nullptr);
			vkFreeMemory(trekDevice.device(), indexBufferMemory, nullptr);
		}
	}

	std::unique_ptr<TrekModel> TrekModel::createModelFromFile(TrekCore& device, const std::string& filePath)
	{
		Data data{};
		data.loadModel(filePath);
		return std::make_unique<TrekModel>(device, data);
	}

	void TrekModel::bind(const VkCommandBuffer commandBuffer) const
	{
		const VkBuffer buffers[] = { vertexBuffer };
		const VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if(hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void TrekModel::draw(const VkCommandBuffer commandBuffer) const
	{
		if(hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void TrekModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be atleast 3");
		const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		trekDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);


		void* data;
		vkMapMemory(trekDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(trekDevice.device(), stagingBufferMemory);

		trekDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory);

		trekDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(trekDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(trekDevice.device(), stagingBufferMemory, nullptr);
	}

	void TrekModel::createIndexBuffer(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer) return;

		const VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		trekDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);


		void* data;
		vkMapMemory(trekDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(trekDevice.device(), stagingBufferMemory);

		trekDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory);

		trekDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(trekDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(trekDevice.device(), stagingBufferMemory, nullptr);
	}

	void TrekModel::Data::loadModel(const std::string& filePath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath.c_str()))
		{
			throw std::runtime_error(err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for(const auto& shape : shapes)
		{
			for(const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};
				if(index.vertex_index >= 0)
				{
					vertex.pos = 
					{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};
					vertex.color = { 1.f, 1.f, 1.f };
				}

				if (index.normal_index >= 0)
				{
					vertex.normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv =
					{
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if(uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

}
