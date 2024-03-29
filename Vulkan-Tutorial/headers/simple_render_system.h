#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H
#include "trek_pipeline.h"
#include "trek_core.h"
#include "trek_game_object.h"
#include "trek_camera.h"
#include "trek_frame_info.h"

// std
#include <memory>
#include <vector>

namespace Trek
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(
			TrekCore& device,
			VkRenderPass renderPass,
			VkDescriptorSetLayout globalSetLayout,
			std::string vertexShader,
			std::string fragmentShader);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(SimpleRenderSystem&) = delete;
		SimpleRenderSystem(const SimpleRenderSystem&&) = delete;
		SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

		void renderGameObjects(
			FrameInfo& frameInfo) const;
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		TrekCore& trekDevice;
		std::unique_ptr<TrekPipeline> trekPipeline;
		VkPipelineLayout pipelineLayout;

		const std::string vertexShaderPath;
		const std::string fragmentShaderPath;
	};
}

#endif
