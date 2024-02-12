#ifndef TREK_PIPELINE_H
#define TREK_PIPELINE_H
#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>

#include "trek_core.h"

namespace Trek
{
    const std::vector<VkDynamicState> DYNAMIC_STATES = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
    };

    struct PipelineConfigInfo
    {
        VkPipelineViewportStateCreateInfo viewport_info;
        std::vector<VkDynamicState> dynamic_states;
        VkPipelineDynamicStateCreateInfo dynamic_state_info;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
        VkPipelineRasterizationStateCreateInfo rasterization_info;
        VkPipelineMultisampleStateCreateInfo multisample_info;
        VkPipelineColorBlendAttachmentState color_blend_attatchment;
        VkPipelineColorBlendStateCreateInfo color_blend_info;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
        VkRenderPass render_pass = VK_NULL_HANDLE;
        uint32_t subpass = 0;
    };

    class TrekPipeline
    {
    public:
        TrekPipeline(
			TrekCore& deviceCore,
            const std::string& vertexFilePath,
            const std::string& fragFilePath,
            const PipelineConfigInfo& configInfo);

        ~TrekPipeline();

        TrekPipeline(const TrekPipeline&) = delete;
        TrekPipeline& operator=(const TrekPipeline&) = delete;

        void Bind(VkCommandBuffer commandBuffer) const;
        static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        static std::vector<char> ReadFile(const std::string& filePath);
        void CreateGraphicsPipeline(
            const std::string& vertexFilePath,
            const std::string& fragFilePath,
            const PipelineConfigInfo& configInfo);

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const;

        TrekCore& m_coreDevice;
        VkPipeline m_graphicsPipeline;
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;
    };

}

#endif
