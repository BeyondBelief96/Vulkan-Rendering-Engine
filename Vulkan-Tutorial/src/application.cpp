#include "application.h"

#include <stdexcept>


namespace Trek
{
	Application::Application()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(trekDevice.device(), pipelineLayout, nullptr);
	}

	void Application::Run() const
	{
		while(!trekWindow.shouldClose())
		{
			glfwPollEvents();
		}
	}

	void Application::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if(vkCreatePipelineLayout(trekDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void Application::createPipeline()
	{
		auto pipelineConfig = 
			TrekPipeline::defaultPipelineConfigInfo(trekSwapChain.width(), trekSwapChain.height());

		pipelineConfig.renderPass = trekSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		trekPipeline = std::make_unique<TrekPipeline>(
			trekDevice,
			"shaders/vert.spv",
			"shaders/frag.spv",
			pipelineConfig);
	}

	void Application::createCommandBuffers()
	{
	}

	void Application::drawFrame()
	{
	}
}

