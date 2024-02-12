#include <vulkan/vulkan_core.h>


/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "trek_buffer.h"
// std
#include <cassert>
#include <cstring>

    namespace Trek {

    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize TrekBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    TrekBuffer::TrekBuffer(
        TrekCore& device,
        const VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        const VkDeviceSize minOffsetAlignment)
        : m_coreDevice{ device },
          m_instanceCount{instanceCount},
          m_instanceSize{instanceSize},
        m_usageFlags{ usageFlags },
        m_memoryPropertyFlags{ memoryPropertyFlags } {
        m_alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
        m_bufferSize = m_alignmentSize * instanceCount;
        device.CreateBuffer(m_bufferSize, usageFlags, memoryPropertyFlags, m_buffer, m_memory);
    }

    TrekBuffer::~TrekBuffer() {
        Unmap();
        vkDestroyBuffer(m_coreDevice.GetDevice(), m_buffer, nullptr);
        vkFreeMemory(m_coreDevice.GetDevice(), m_memory, nullptr);
    }

    /**
     * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    VkResult TrekBuffer::Map(const VkDeviceSize size, const VkDeviceSize offset) {
        assert(m_buffer && m_memory && "Called map on buffer before create");
        return vkMapMemory(m_coreDevice.GetDevice(), m_memory, offset, size, 0, &m_mapped);
    }

    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void TrekBuffer::Unmap() {
        if (m_mapped) {
            vkUnmapMemory(m_coreDevice.GetDevice(), m_memory);
            m_mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void TrekBuffer::WriteToBuffer(const void* data, const VkDeviceSize size, const VkDeviceSize offset) const
    {
        assert(m_mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(m_mapped, data, m_bufferSize);
        }
        else {
	        auto memOffset = static_cast<char*>(m_mapped);
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult TrekBuffer::Flush(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_coreDevice.GetDevice(), 1, &mappedRange);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult TrekBuffer::Invalidate(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_coreDevice.GetDevice(), 1, &mappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo TrekBuffer::DescriptorInfo(const VkDeviceSize size, const VkDeviceSize offset) const
    {
        return VkDescriptorBufferInfo{
            m_buffer,
            offset,
            size,
        };
    }

    /**
     * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void TrekBuffer::WriteToIndex(const void* data, const int index) const
    {
        WriteToBuffer(data, m_instanceSize, index * m_alignmentSize);
    }

    /**
     *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult TrekBuffer::FlushIndex(const int index) const { return Flush(m_alignmentSize, index * m_alignmentSize); }

    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo TrekBuffer::DescriptorInfoForIndex(const int index) const
    {
        return DescriptorInfo(m_alignmentSize, index * m_alignmentSize);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult TrekBuffer::InvalidateIndex(const int index) const
    {
        return Invalidate(m_alignmentSize, index * m_alignmentSize);
    }

}  // namespace lve
