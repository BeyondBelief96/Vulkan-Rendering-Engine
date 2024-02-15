#include "trek_renderer.h"

// std
#include <array>
#include <stdexcept>


namespace Trek
{
	TrekRenderer::TrekRenderer(TrekWindow& window, TrekCore& device)
		: trekWindow(window), trekDevice(device)
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	TrekRenderer::~TrekRenderer()
	{
		freeCommandBuffers();
	}

	void TrekRenderer::recreateSwapChain()
	{
		auto extent = trekWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = trekWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(trekDevice.device());

		if (trekSwapChain == nullptr)
		{
			trekSwapChain = std::make_unique<TrekSwapChain>(trekDevice, extent);
		}
		else
		{
			std::shared_ptr<TrekSwapChain> oldSwapChain = std::move(trekSwapChain);
			trekSwapChain = std::make_unique<TrekSwapChain>(trekDevice, extent, oldSwapChain);
			if(!oldSwapChain->compareSwapFormats(*trekSwapChain))
			{
				throw std::runtime_error("Swap chain image or depth format has changed!");
			}
		}

		// TODO: come back to this.
	}


	void TrekRenderer::createCommandBuffers()
	{
		assert(trekSwapChain != nullptr && "Cannot create pipeline before swap chain.");
		commandBuffers.resize(TrekSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = trekDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(trekDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void TrekRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			trekDevice.device(),
			trekDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());

		commandBuffers.clear();
	}

	VkCommandBuffer TrekRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Cannot call begin frame while frame is in progress.");
		const auto result = trekSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		const auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void TrekRenderer::endFrame()
	{
		assert(isFrameStarted && "Cannot call end frame while frame is not in progress.");
		const auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}

		const auto result = trekSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			trekWindow.wasWindowResized())
		{
			trekWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % TrekSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void TrekRenderer::beginSwapChainRenderPass(const VkCommandBuffer commandBuffer) const
	{
		assert(isFrameStarted && "Cannot call beginSwapChainRenderPass while frame is in progress.");
		assert(commandBuffer == getCurrentCommandBuffer() 
			&& "Cannot begin render pass on command buffer from a different frame.");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = trekSwapChain->getRenderPass();
		renderPassInfo.framebuffer = trekSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = trekSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(trekSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(trekSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = trekSwapChain->getSwapChainExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void TrekRenderer::endSwapChainRenderPass(const VkCommandBuffer commandBuffer) const
	{
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass while frame is in progress.");
		assert(commandBuffer == getCurrentCommandBuffer()
			&& "Cannot end render pass on command buffer from a different frame.");

		vkCmdEndRenderPass(commandBuffer);
	}


}

