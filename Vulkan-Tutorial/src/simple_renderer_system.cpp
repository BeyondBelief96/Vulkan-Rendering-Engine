#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <stdexcept>


namespace Trek
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{};
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem::SimpleRenderSystem(TrekCore& device, const VkRenderPass renderPass)
		: trekDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(trekDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(trekDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void SimpleRenderSystem::createPipeline(const VkRenderPass renderPass)
	{
		PipelineConfigInfo pipelineConfigInfo{};
		TrekPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);

		pipelineConfigInfo.renderPass = renderPass;
		pipelineConfigInfo.pipelineLayout = pipelineLayout;
		trekPipeline = std::make_unique<TrekPipeline>(
			trekDevice,
			"shaders/vert.spv",
			"shaders/frag.spv",
			pipelineConfigInfo);
	}

	void SimpleRenderSystem::renderGameObjects(
		const VkCommandBuffer commandBuffer,
		std::vector<TrekGameObject>& gameObjects,
		const TrekCamera& camera) const
	{
		trekPipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();
		for (auto& obj : gameObjects)
		{
			SimplePushConstantData push{};
			push.color = obj.color;
			push.transform = projectionView * obj.transform2d.mat4();
			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

}

