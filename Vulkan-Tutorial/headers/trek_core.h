#ifndef TREK_CORE_H
#define TREK_CORE_H
#include "trek_window.h"

// std lib headers
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Trek {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class TrekCore {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        TrekCore(TrekWindow& window);
        ~TrekCore();

        // Not copyable or movable
        TrekCore(const TrekCore&) = delete;
        TrekCore operator=(const TrekCore&) = delete;
        TrekCore(TrekCore&&) = delete;
        TrekCore& operator=(TrekCore&&) = delete;

        VkCommandPool getCommandPool() const { return commandPool; }
        VkDevice device() const { return device_; }
        VkSurfaceKHR surface() const { return surface_; }
        VkQueue graphicsQueue() const { return graphicsQueue_; }
        VkQueue presentQueue() const { return presentQueue_; }

        SwapChainSupportDetails getSwapChainSupport() const { return querySwapChainSupport(physicalDevice); }
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        QueueFamilyIndices findPhysicalQueueFamilies() const { return findQueueFamilies(physicalDevice); }
        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

        // Buffer Helper Functions
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory) const;
        VkCommandBuffer beginSingleTimeCommands() const;
        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
        void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) const;

        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags memoryProperties,
            VkImage& image,
            VkDeviceMemory& imageMemory) const;

        VkPhysicalDeviceProperties properties;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device) const;
        std::vector<const char*> getRequiredExtensions() const;
        bool checkValidationLayerSupport() const;
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void hasGflwRequiredInstanceExtensions() const;
        bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        TrekWindow& window;
        VkCommandPool commandPool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphicsQueue_;
        VkQueue presentQueue_;

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };

}

#endif