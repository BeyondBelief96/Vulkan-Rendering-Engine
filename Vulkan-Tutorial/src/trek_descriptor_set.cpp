#include "trek_descriptor_set.h"

#include <cassert>

namespace Trek
{
    // *************** Descriptor Set Layout *********************

    TrekDescriptorSetLayout::Builder& TrekDescriptorSetLayout::Builder::AddBinding(
        const uint32_t binding,
        const VkDescriptorType descriptorType,
        const VkShaderStageFlags stageFlags,
        const uint32_t count) {
        assert(m_bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        m_bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<TrekDescriptorSetLayout>TrekDescriptorSetLayout::Builder::Build() const
    {
        return std::make_unique<TrekDescriptorSetLayout>(m_coreDevice, m_bindings);
    }

   TrekDescriptorSetLayout::TrekDescriptorSetLayout(
        TrekCore& coreDevice, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings)
        : m_coreDevice(coreDevice), m_bindings{ bindings } {
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
            coreDevice.GetDevice(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
   }

   TrekDescriptorSetLayout::~TrekDescriptorSetLayout()
   {
        vkDestroyDescriptorSetLayout(m_coreDevice.GetDevice(), m_descriptorSetLayout, nullptr);
   }

   // *************** Descriptor Pool Builder *********************

    TrekDescriptorPool::Builder& TrekDescriptorPool::Builder::AddPoolSize(
	    const VkDescriptorType descriptorType, const uint32_t count) {
        m_poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    TrekDescriptorPool::Builder& TrekDescriptorPool::Builder::SetPoolFlags(
	    const VkDescriptorPoolCreateFlags flags) {
        m_poolFlags = flags;
        return *this;
    }
    TrekDescriptorPool::Builder& TrekDescriptorPool::Builder::SetMaxSets(uint32_t count) {
        m_maxSets = count;
        return *this;
    }

    std::unique_ptr<TrekDescriptorPool> TrekDescriptorPool::Builder::Build() const {
        return std::make_unique<TrekDescriptorPool>(m_coreDevice, m_maxSets, m_poolFlags, m_poolSizes);
    }

    // *************** Descriptor Pool *********************

    TrekDescriptorPool::TrekDescriptorPool(
        TrekCore& coreDevice,
        const uint32_t maxSets,
        const VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : m_coreDevice{ coreDevice } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(coreDevice.GetDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    TrekDescriptorPool::~TrekDescriptorPool() {
        vkDestroyDescriptorPool(m_coreDevice.GetDevice(), m_descriptorPool, nullptr);
    }

    bool TrekDescriptorPool::AllocateDescriptorSet(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_coreDevice.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void TrekDescriptorPool::FreeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            m_coreDevice.GetDevice(),
            m_descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void TrekDescriptorPool::ResetPool() const
    {
        vkResetDescriptorPool(m_coreDevice.GetDevice(), m_descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    TrekDescriptorWriter::TrekDescriptorWriter(TrekDescriptorSetLayout& setLayout, TrekDescriptorPool& pool)
        : m_setLayout{ setLayout }, m_pool{ pool } {}

    TrekDescriptorWriter& TrekDescriptorWriter::WriteBuffer(
	    const uint32_t binding, const VkDescriptorBufferInfo* bufferInfo) {
        assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

        const auto& bindingDescription = m_setLayout.m_bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    TrekDescriptorWriter& TrekDescriptorWriter::WriteImage(
	    const uint32_t binding, const VkDescriptorImageInfo* imageInfo) {
        assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_setLayout.m_bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    bool TrekDescriptorWriter::Build(VkDescriptorSet& set) {
	    const bool success = m_pool.AllocateDescriptorSet(m_setLayout.GetDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        Overwrite(set);
        return true;
    }

    void TrekDescriptorWriter::Overwrite(const VkDescriptorSet& set) {
        for (auto& write : m_writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_pool.m_coreDevice.GetDevice(), m_writes.size(), m_writes.data(), 0, nullptr);
    }
}
