#ifndef TREK_PIPELINE_H
#define TREK_PIPELINE_H
#include <string>
#include <vector>

#include "trek_core.h"


namespace Trek
{
	struct PipelineConfigInfo
	{
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttatchment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t subpass = 0;
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

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

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
