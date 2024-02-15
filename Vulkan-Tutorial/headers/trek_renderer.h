#ifndef TREK_RENDERER_H
#define TREK_RENDERER_H
#include "trek_window.h"
#include "trek_core.h"
#include "trek_swapchain.h"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace Trek
{
	class TrekRenderer
	{
	public:
		TrekRenderer(TrekWindow& window, TrekCore& device);
		~TrekRenderer();
		TrekRenderer(const TrekRenderer&) = delete;
		TrekRenderer& operator=(TrekRenderer&) = delete;
		TrekRenderer(const TrekRenderer&&) = delete;
		TrekRenderer& operator=(TrekRenderer&&) = delete;


		VkRenderPass getSwapChainRenderPass() const { return trekSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress.");
			return commandBuffers[currentFrameIndex];
		}
		int getFrameIndex() const
		{
			assert(isFrameStarted && "Cannot get frame index when frame not in progress!");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer) const;
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		TrekWindow& trekWindow;
		TrekCore& trekDevice;
		std::unique_ptr<TrekSwapChain> trekSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex{0};
		int currentFrameIndex{ 0 };
		bool isFrameStarted = false;
	};
}

#endif
