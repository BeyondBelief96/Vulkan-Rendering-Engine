#ifndef VULKAN_CORE_H
#define VULKAN_CORE_H
#include <optional>
#include <set>
#include <vulkan/vulkan_core.h>

#include "glfw_core.h"

namespace Trek
{
    const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;
        std::optional<uint32_t> transfer_family;

        bool IsComplete() const
        {
            return graphics_family.has_value() && present_family.has_value() &&
                transfer_family.has_value();
        }
    };

    static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    class TrekCore
    {
    public:
	    explicit TrekCore(const WindowCore* surfaceCreator);
        TrekCore(TrekCore& core) = delete;
        TrekCore& operator=(const TrekCore& core) = delete;
        ~TrekCore();

        void Init(const WindowCore* surfaceCreator);

        VkInstance GetInstance() const { return m_instance; }
        VkSurfaceKHR GetSurface() const { return m_surface; }
        VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
        VkDevice GetDevice() const { return m_device; }
        QueueFamilyIndices GetQueueFamilies() const{ return m_indices; }
        VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
        VkQueue GetPresentQueue() const { return m_presentQueue; }
        VkQueue GetTransferQueue() const { return m_transferQueue; }
        VkCommandPool GetGraphicsCmdPool() const { return m_graphicsCommandPool; }
        VkCommandPool GetTransferCmdPool() const { return m_transferCommandPool; }

        VkPhysicalDeviceProperties properties;

        uint32_t FindBufferMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags properties,
            VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;

        static void CopyBuffer(const VkDevice& device, const VkDeviceSize size, VkBuffer srcBuffer, VkBuffer dstBuffer, const VkCommandPool& commandPool,
            const VkQueue& commandQueue);

    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface(const WindowCore* surfaceCreator);
    	void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();

        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR& surface);
        bool IsDeviceSuitable(const VkPhysicalDevice device);

        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        QueueFamilyIndices m_indices;
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;
        VkQueue m_transferQueue = VK_NULL_HANDLE;
        VkCommandPool m_graphicsCommandPool = VK_NULL_HANDLE;
        VkCommandPool m_transferCommandPool = VK_NULL_HANDLE;
    };
}

#endif
