#ifndef TREK_SWAP_CHAIN_H
#define TREK_SWAP_CHAIN_H
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "trek_core.h"

namespace Trek
{
    class TrekSwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        TrekSwapChain(TrekCore& deviceRef, VkExtent2D windowExtent);
        TrekSwapChain(TrekCore& deviceRef, VkExtent2D windowExtent, const std::shared_ptr<TrekSwapChain>& previous);
        ~TrekSwapChain();

        TrekSwapChain(const TrekSwapChain&) = delete;
        TrekSwapChain operator=(const TrekSwapChain&) = delete;

        VkFramebuffer getFrameBuffer(const int index) const { return swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() const { return renderPass; }
        VkImageView getImageView(const int index) const { return swapChainImageViews[index]; }
        size_t imageCount() const { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() const { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() const { return swapChainExtent; }
        uint32_t width() const { return swapChainExtent.width; }
        uint32_t height() const { return swapChainExtent.height; }

        float extentAspectRatio() const
        {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }
        VkFormat findDepthFormat() const;

        VkResult acquireNextImage(uint32_t* imageIndex) const;
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, const uint32_t* imageIndex);
        bool compareSwapFormats(const TrekSwapChain& sc) const;

    private:

        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        TrekCore& device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<TrekSwapChain> oldSwapchain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };
}

#endif
