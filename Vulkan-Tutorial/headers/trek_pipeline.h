#ifndef TREK_PIPELINE_H
#define TREK_PIPELINE_H
#include <string>
#include <vector>

#include "trek_core.h"
#include "trek_model.h"


namespace Trek
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(PipelineConfigInfo&) = delete;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttatchment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t subpass = 0;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	};

	class TrekPipeline
	{
	public:
		TrekPipeline(
			TrekCore& device,
			const std::string& vertexFilePath,
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo);

		~TrekPipeline();
		TrekPipeline(const TrekPipeline&) = delete;
		TrekPipeline& operator=(TrekPipeline&&) = delete;
		TrekPipeline(const TrekPipeline&&) = delete;
		TrekPipeline& operator=(TrekPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer) const;
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filePath);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const;
		void createGraphicsPipeline(
			const TrekCore& device,
			const std::string& vertexFilePath,
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo);

		TrekCore& coreDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}

#endif
