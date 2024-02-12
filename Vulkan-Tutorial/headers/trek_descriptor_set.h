#ifndef TREK_DESCRIPTOR_SET_H
#define TREK_DESCRIPTOR_SET_H
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "trek_core.h"

namespace Trek
{
    class TrekDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(TrekCore& coreDevice) : m_coreDevice{ coreDevice } {}

            Builder& AddBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<TrekDescriptorSetLayout> Build() const;

        private:
            TrekCore& m_coreDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
        };

        TrekDescriptorSetLayout(
            TrekCore& coreDevice, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings);
        ~TrekDescriptorSetLayout();
        TrekDescriptorSetLayout(const TrekDescriptorSetLayout&) = delete;
        TrekDescriptorSetLayout& operator=(const TrekDescriptorSetLayout&) = delete;

    	VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descriptorSetLayout; }

    private:
        TrekCore& m_coreDevice;
        VkDescriptorSetLayout m_descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

        friend class TrekDescriptorWriter;
    };

    class TrekDescriptorPool {
    public:
        class Builder {
        public:
            Builder(TrekCore& coreDevice) : m_coreDevice{ coreDevice } {}

            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& SetMaxSets(uint32_t count);
            std::unique_ptr<TrekDescriptorPool> Build() const;

        private:
            TrekCore& m_coreDevice;
            std::vector<VkDescriptorPoolSize> m_poolSizes{};
            uint32_t m_maxSets = 1000;
            VkDescriptorPoolCreateFlags m_poolFlags = 0;
        };

        TrekDescriptorPool(
            TrekCore& coreDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~TrekDescriptorPool();
        TrekDescriptorPool(const TrekDescriptorPool&) = delete;
        TrekDescriptorPool& operator=(const TrekDescriptorPool&) = delete;

        bool AllocateDescriptorSet(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void FreeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const;

        void ResetPool() const;

    private:
        TrekCore& m_coreDevice;
        VkDescriptorPool m_descriptorPool;

        friend class TrekDescriptorWriter;
    };

    class TrekDescriptorWriter {
    public:
        TrekDescriptorWriter(TrekDescriptorSetLayout& setLayout, TrekDescriptorPool& pool);

        TrekDescriptorWriter& WriteBuffer(uint32_t binding, const VkDescriptorBufferInfo* bufferInfo);
        TrekDescriptorWriter& WriteImage(uint32_t binding, const VkDescriptorImageInfo* imageInfo);

        bool Build(VkDescriptorSet& set);
        void Overwrite(const VkDescriptorSet& set);

    private:
        TrekDescriptorSetLayout& m_setLayout;
        TrekDescriptorPool& m_pool;
        std::vector<VkWriteDescriptorSet> m_writes;
    };
}

#endif
