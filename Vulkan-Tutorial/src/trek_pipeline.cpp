#include "trek_pipeline.h"

#include <fstream>
#include <stdexcept>

#include "trek_model.h"

namespace Trek
{
	TrekPipeline::TrekPipeline(
		TrekCore& deviceCore,
		const std::string& vertexFilePath,
		const std::string& fragFilePath,
		const PipelineConfigInfo& configInfo) : m_coreDevice(deviceCore)
	{
		CreateGraphicsPipeline(vertexFilePath, fragFilePath, configInfo);
	}

	TrekPipeline::~TrekPipeline()
	{
		vkDestroyShaderModule(m_coreDevice.GetDevice(), m_fragShaderModule, nullptr);
		vkDestroyShaderModule(m_coreDevice.GetDevice(), m_vertShaderModule, nullptr);
		vkDestroyPipeline(m_coreDevice.GetDevice(), m_graphicsPipeline, nullptr);
	}

	std::vector<char> TrekPipeline::ReadFile(const std::string& filePath)
	{
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Failed to open file!");

		const size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
		file.close();
		return buffer;

	}

	void TrekPipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		if (vkCreateShaderModule(m_coreDevice.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
	}

	void TrekPipeline::CreateGraphicsPipeline(
		const std::string& vertexFilePath,
		const std::string& fragFilePath,
		const PipelineConfigInfo& configInfo)
	{
		const auto vertShaderCode = ReadFile(vertexFilePath);
		const auto fragShaderCode = ReadFile(fragFilePath);

		CreateShaderModule(vertShaderCode, &m_vertShaderModule);
		CreateShaderModule(fragShaderCode, &m_fragShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_vertShaderModule;
		vertShaderStageInfo.pName = "main";
		
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_fragShaderModule;
		fragShaderStageInfo.pName = "main";

		shaderStages[0] = vertShaderStageInfo;
		shaderStages[1] = fragShaderStageInfo;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		const auto bindingDescription = TrekModel::Vertex::GetBindingDescription();
		const auto attributeDescriptions = TrekModel::Vertex::GetAttributeDescriptions();

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.input_assembly_info;
		pipelineInfo.pViewportState = &configInfo.viewport_info;
		pipelineInfo.pRasterizationState = &configInfo.rasterization_info;
		pipelineInfo.pMultisampleState = &configInfo.multisample_info;
		pipelineInfo.pDepthStencilState = &configInfo.depth_stencil_info; // Optional
		pipelineInfo.pColorBlendState = &configInfo.color_blend_info;
		pipelineInfo.pDynamicState = &configInfo.dynamic_state_info;
		pipelineInfo.layout = configInfo.pipeline_layout;
		pipelineInfo.renderPass = configInfo.render_pass;
		pipelineInfo.subpass = configInfo.subpass;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(m_coreDevice.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}
	}

	void TrekPipeline::Bind(const VkCommandBuffer commandBuffer) const
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
	}

	void TrekPipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
	{
		configInfo.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(DYNAMIC_STATES.size());
		configInfo.dynamic_state_info.pDynamicStates = DYNAMIC_STATES.data();

		configInfo.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.input_assembly_info.primitiveRestartEnable = VK_FALSE;

		configInfo.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewport_info.viewportCount = 1;
		configInfo.viewport_info.scissorCount = 1;
		configInfo.viewport_info.pScissors = nullptr;
		configInfo.viewport_info.pViewports = nullptr;

		configInfo.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterization_info.depthClampEnable = VK_FALSE;
		configInfo.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterization_info.lineWidth = 1.0f;
		configInfo.rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
		configInfo.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterization_info.depthBiasEnable = VK_FALSE;
		configInfo.rasterization_info.depthBiasConstantFactor = 0.0f; // Optional
		configInfo.rasterization_info.depthBiasClamp = 0.0f; // Optional
		configInfo.rasterization_info.depthBiasSlopeFactor = 0.0f; // Optional

		configInfo.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisample_info.sampleShadingEnable = VK_FALSE;
		configInfo.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisample_info.minSampleShading = 1.0f; // Optional
		configInfo.multisample_info.pSampleMask = nullptr; // Optional
		configInfo.multisample_info.alphaToCoverageEnable = VK_FALSE; // Optional
		configInfo.multisample_info.alphaToOneEnable = VK_FALSE; // Optional

		configInfo.color_blend_attatchment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.color_blend_attatchment.blendEnable = VK_FALSE;
		configInfo.color_blend_attatchment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		configInfo.color_blend_attatchment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		configInfo.color_blend_attatchment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		configInfo.color_blend_attatchment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		configInfo.color_blend_attatchment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		configInfo.color_blend_attatchment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		configInfo.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.color_blend_info.logicOpEnable = VK_FALSE;
		configInfo.color_blend_info.logicOp = VK_LOGIC_OP_COPY; // Optional
		configInfo.color_blend_info.attachmentCount = 1;
		configInfo.color_blend_info.pAttachments = &configInfo.color_blend_attatchment;
		configInfo.color_blend_info.blendConstants[0] = 0.0f; // Optional
		configInfo.color_blend_info.blendConstants[1] = 0.0f; // Optional
		configInfo.color_blend_info.blendConstants[2] = 0.0f; // Optional
		configInfo.color_blend_info.blendConstants[3] = 0.0f; // Optional

		configInfo.dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamic_states.size());
		configInfo.dynamic_state_info.flags = 0;
	}
}
