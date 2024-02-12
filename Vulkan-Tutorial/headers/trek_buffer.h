#ifndef TREK_BUFFER_H
#define TREK_BUFFER_H
#include <vulkan/vulkan_core.h>

#include "trek_core.h"

namespace Trek
{
    class TrekBuffer {
    public:
        TrekBuffer(
            TrekCore& device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);
        ~TrekBuffer();

        TrekBuffer(const TrekBuffer&) = delete;
        TrekBuffer& operator=(const TrekBuffer&) = delete;

        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void Unmap();

        void WriteToBuffer(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
        VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;

        void WriteToIndex(const void* data, int index) const;
        VkResult FlushIndex(int index) const;
        VkDescriptorBufferInfo DescriptorInfoForIndex(int index) const;
        VkResult InvalidateIndex(int index) const;

        VkBuffer GetBuffer() const { return m_buffer; }
        void* GetMappedMemory() const { return m_mapped; }
        uint32_t GetInstanceCount() const { return m_instanceCount; }
        VkDeviceSize GetInstanceSize() const { return m_instanceSize; }
        VkDeviceSize GetAlignmentSize() const { return m_instanceSize; }
        VkBufferUsageFlags GetUsageFlags() const { return m_usageFlags; }
        VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
        VkDeviceSize GetBufferSize() const { return m_bufferSize; }

    private:
        static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        TrekCore& m_coreDevice;
        void* m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_bufferSize;
        uint32_t m_instanceCount;
        VkDeviceSize m_instanceSize;
        VkDeviceSize m_alignmentSize;
        VkBufferUsageFlags m_usageFlags;
        VkMemoryPropertyFlags m_memoryPropertyFlags;
    };
}

#endif
