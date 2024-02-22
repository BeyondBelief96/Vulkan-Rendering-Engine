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
		glm::mat4 modelMatrix{};
		glm::mat4 normalMatrix{};
	};

	SimpleRenderSystem::SimpleRenderSystem(
		TrekCore& device,
		const VkRenderPass renderPass,
		VkDescriptorSetLayout globalSetLayout)
		: trekDevice{device}
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(trekDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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
		FrameInfo& frameInfo,
		std::vector<TrekGameObject>& gameObjects) const
	{
		trekPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr
		);

		for (auto& obj : gameObjects)
		{
			SimplePushConstantData push{};
			auto modelMatrix = obj.transform2d.mat4();
			push.modelMatrix = obj.transform2d.mat4();
			push.normalMatrix = obj.transform2d.normalMatrix();
			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}

}

