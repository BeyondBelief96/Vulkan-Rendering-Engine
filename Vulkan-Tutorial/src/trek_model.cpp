#include "trek_model.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include "trek_swap_chain.h"

namespace Trek
{
	TrekModel::TrekModel(
		TrekCore& coreDevice,
		const Data& data)
		: m_coreDevice(coreDevice)
	{
		CreateVertexBuffers(data.vertices);
		CreateIndexBuffer(data.indices);
	}

	TrekModel::~TrekModel() = default;

	void TrekModel::Bind(const VkCommandBuffer commandBuffer) const
	{
		const VkBuffer vertexBuffers[] = { m_vertexBuffer->GetBuffer() };
		const VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		if(m_hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void TrekModel::Draw(const VkCommandBuffer commandBuffer) const
	{
		if(m_hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indexCount), 1, 0, 0, 0);
		}
	}

	void TrekModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_vertexCount >= 3 && "Vertex count must be at least 3.");
		const VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		TrekBuffer stagingBuffer {
			m_coreDevice,
			vertexSize,
			m_vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)vertices.data());

		m_vertexBuffer = std::make_unique<TrekBuffer>(
			m_coreDevice,
			vertexSize,
			m_vertexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		TrekCore::CopyBuffer(
			m_coreDevice.GetDevice(),
			bufferSize,
			stagingBuffer.GetBuffer(),
			m_vertexBuffer->GetBuffer(),
			m_coreDevice.GetTransferCmdPool(),
			m_coreDevice.GetTransferQueue());
	}

	void TrekModel::CreateIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_indexCount = static_cast<uint32_t>(indices.size());
		m_hasIndexBuffer = m_indexCount > 0;
		if (!m_hasIndexBuffer) return;

		const VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		TrekBuffer stagingBuffer{
			m_coreDevice,
			indexSize,
			m_indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)indices.data());

		m_indexBuffer = std::make_unique<TrekBuffer>(
			m_coreDevice,
			indexSize,
			m_indexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		TrekCore::CopyBuffer(
			m_coreDevice.GetDevice(),
			bufferSize, 
			stagingBuffer.GetBuffer(),
			m_indexBuffer->GetBuffer(),
			m_coreDevice.GetTransferCmdPool(),
			m_coreDevice.GetTransferQueue());
	}
}
