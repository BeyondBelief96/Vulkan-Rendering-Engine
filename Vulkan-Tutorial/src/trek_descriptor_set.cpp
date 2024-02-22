#include "trek_descriptor_set.h"

#include <cassert>
#include <stdexcept>

namespace Trek
{
    // *************** Descriptor Set Layout *********************

    TrekDescriptorSetLayout::Builder& TrekDescriptorSetLayout::Builder::addBinding(
        const uint32_t binding,
        const VkDescriptorType descriptorType,
        const VkShaderStageFlags stageFlags,
        const uint32_t count) {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<TrekDescriptorSetLayout>TrekDescriptorSetLayout::Builder::build() const
    {
        return std::make_unique<TrekDescriptorSetLayout>(trekDevice, bindings);
    }

    TrekDescriptorSetLayout::TrekDescriptorSetLayout(
        TrekCore& trekDevice, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings)
        : trekDevice(trekDevice), bindings{ bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        setLayoutBindings.reserve(bindings.size());
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            trekDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    TrekDescriptorSetLayout::~TrekDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(trekDevice.device(), descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    TrekDescriptorPool::Builder& TrekDescriptorPool::Builder::addPoolSize(
        const VkDescriptorType descriptorType, const uint32_t count) {
        poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    TrekDescriptorPool::Builder& TrekDescriptorPool::Builder::setPoolFlags(
        const VkDescriptorPoolCreateFlags flags) {
        poolFlags = flags;
        return *this;
    }
    TrekDescriptorPool::Builder& TrekDescriptorPool::Builder::setMaxSets(uint32_t count) {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<TrekDescriptorPool> TrekDescriptorPool::Builder::build() const {
        return std::make_unique<TrekDescriptorPool>(trekDevice, maxSets, poolFlags, poolSizes);
    }

    // *************** Descriptor Pool *********************

    TrekDescriptorPool::TrekDescriptorPool(
        TrekCore& trekDevice,
        const uint32_t maxSets,
        const VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : trekDevice{ trekDevice } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(trekDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    TrekDescriptorPool::~TrekDescriptorPool() {
        vkDestroyDescriptorPool(trekDevice.device(), descriptorPool, nullptr);
    }

    bool TrekDescriptorPool::allocateDescriptorSet(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(trekDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void TrekDescriptorPool::freeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            trekDevice.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void TrekDescriptorPool::resetPool() const
    {
        vkResetDescriptorPool(trekDevice.device(), descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    TrekDescriptorWriter::TrekDescriptorWriter(TrekDescriptorSetLayout& setLayout, TrekDescriptorPool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    TrekDescriptorWriter& TrekDescriptorWriter::writeBuffer(
        const uint32_t binding, const VkDescriptorBufferInfo* bufferInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        const auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    TrekDescriptorWriter& TrekDescriptorWriter::writeImage(
        const uint32_t binding, const VkDescriptorImageInfo* imageInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool TrekDescriptorWriter::build(VkDescriptorSet& set) {
        const bool success = pool.allocateDescriptorSet(setLayout.GetDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void TrekDescriptorWriter::overwrite(const VkDescriptorSet& set) {
        for (auto& write : writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.trekDevice.device(), writes.size(), writes.data(), 0, nullptr);
    }
}