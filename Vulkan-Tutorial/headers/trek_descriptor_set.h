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
            Builder(TrekCore& trekDevice) : trekDevice{ trekDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<TrekDescriptorSetLayout> build() const;

        private:
            TrekCore& trekDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        TrekDescriptorSetLayout(
            TrekCore& trekDevice, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings);
        ~TrekDescriptorSetLayout();
        TrekDescriptorSetLayout(const TrekDescriptorSetLayout&) = delete;
        TrekDescriptorSetLayout& operator=(const TrekDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        TrekCore& trekDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class TrekDescriptorWriter;
    };

    class TrekDescriptorPool {
    public:
        class Builder {
        public:
            Builder(TrekCore& trekDevice) : trekDevice{ trekDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<TrekDescriptorPool> build() const;

        private:
            TrekCore& trekDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        TrekDescriptorPool(
            TrekCore& trekDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~TrekDescriptorPool();
        TrekDescriptorPool(const TrekDescriptorPool&) = delete;
        TrekDescriptorPool& operator=(const TrekDescriptorPool&) = delete;

        bool allocateDescriptorSet(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool() const;

    private:
        TrekCore& trekDevice;
        VkDescriptorPool descriptorPool;

        friend class TrekDescriptorWriter;
    };

    class TrekDescriptorWriter {
    public:
        TrekDescriptorWriter(TrekDescriptorSetLayout& setLayout, TrekDescriptorPool& pool);

        TrekDescriptorWriter& writeBuffer(uint32_t binding, const VkDescriptorBufferInfo* bufferInfo);
        TrekDescriptorWriter& writeImage(uint32_t binding, const VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(const VkDescriptorSet& set);

    private:
        TrekDescriptorSetLayout& setLayout;
        TrekDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}

#endif