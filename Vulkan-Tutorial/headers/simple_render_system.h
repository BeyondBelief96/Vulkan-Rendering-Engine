#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H
#include "trek_pipeline.h"
#include "trek_core.h"
#include "trek_game_object.h"

// std
#include <memory>
#include <vector>

namespace Trek
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(TrekCore& device, VkRenderPass renderPass);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(SimpleRenderSystem&) = delete;
		SimpleRenderSystem(const SimpleRenderSystem&&) = delete;
		SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<TrekGameObject>& gameObjects) const;
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		TrekCore& trekDevice;
		std::unique_ptr<TrekPipeline> trekPipeline;
		VkPipelineLayout pipelineLayout;
	};
}

#endif
