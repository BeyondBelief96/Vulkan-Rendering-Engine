#ifndef APPLICATION_H
#define APPLICATION_H
#include "trek_window.h"
#include "trek_pipeline.h"
#include "trek_core.h"
#include "trek_swapchain.h"

// std
#include <memory>
#include <vector>

namespace Trek
{
	class Application
	{
	public:
		Application();
		~Application();
		Application(const Application&) = delete;
		Application& operator=(Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(Application&&) = delete;
		void Run() const;
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();


		TrekWindow trekWindow{WIDTH, HEIGHT, "Vulkan Tutorial!"};
		TrekCore trekDevice{ trekWindow };
		TrekSwapChain trekSwapChain{ trekDevice, trekWindow.getExtent() };
		std::unique_ptr<TrekPipeline> trekPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffer;
	};
}

#endif
