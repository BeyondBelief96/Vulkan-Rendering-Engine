#ifndef TREK_SWAP_CHAIN_H
#define TREK_SWAP_CHAIN_H

#include <vector>
#include <vulkan/vulkan_core.h>
#include <cstdint> // Necessary for uint32_t
#include "trek_core.h"
namespace Trek
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	inline SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.present_modes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.present_modes.data());
		}

		return details;
	}

	class TrekSwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		TrekSwapChain(TrekCore& deviceCore, GLFWCore& glfwCore);
		TrekSwapChain(
			TrekCore& deviceCore,
			GLFWCore& glfwCore,
			std::shared_ptr<TrekSwapChain> previous);
		~TrekSwapChain();

		TrekSwapChain(const TrekSwapChain&) = delete;
		TrekSwapChain& operator=(const TrekSwapChain&) = delete;

		VkSwapchainKHR GetSwapChain() const { return m_swapChain; }
		VkFramebuffer GetFrameBuffer(const int index) const { return m_swapChainFramebuffers[index]; }
		VkRenderPass GetRenderPass() const { return m_renderPass; }
		VkImageView GetImageView(const int index) const { return m_swapChainImageViews[index]; }
		size_t ImageCount() const { return m_swapChainImages.size(); }
		VkFormat GetSwapChainImageFormat() const { return m_swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() const { return m_swapChainExtent; }
		uint32_t Width() const { return m_swapChainExtent.width; }
		uint32_t Height() const { return m_swapChainExtent.height; }
		size_t GetCurrentFrame() const { return m_currentFrame; }

		float ExtentAspectRatio() const {
			return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
		}

		VkResult AcquireNextImage(uint32_t* imageIndex) const;
		VkResult SubmitCommandBuffers(const VkCommandBuffer& buffers, const uint32_t* imageIndex);

		void CleanupSwapChain() const;

	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateSyncObjects();

		// Helper functions
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkRenderPass m_renderPass;

		std::vector<VkImage> m_depthImages;
		std::vector<VkDeviceMemory> m_depthImageMemorys;
		std::vector<VkImageView> m_depthImageViews;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;

		GLFWCore& m_glfwCore;
		TrekCore& m_coreDevice;

		VkSwapchainKHR m_swapChain;
		std::shared_ptr<TrekSwapChain> m_oldSwapChain;

		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		size_t m_currentFrame = 0;
	};
}

#endif