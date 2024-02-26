#include "scene.h"

namespace Trek
{
	Scene::Scene(
		TrekWindow& trekWindow,
		TrekCore& trekDevice,
		std::string vertexShaderFilePath,
		std::string fragmentShaderFilePath) :
		trekWindow(trekWindow),
		trekDevice(trekDevice),
		vertexShaderPath(vertexShaderFilePath),
		fragmentShaderPath(fragmentShaderFilePath)
	{
		globalPool = TrekDescriptorPool::Builder(trekDevice)
			.setMaxSets(TrekSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, TrekSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		// Setting up global uniform buffers.
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<TrekBuffer>(
				trekDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

			uboBuffers[i]->map();
		}

		// Setting up descriptor sets
		globalDescriptorSetLayout = TrekDescriptorSetLayout::Builder(trekDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			TrekDescriptorWriter(*globalDescriptorSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		renderSystem = std::make_unique<SimpleRenderSystem>(
			trekDevice,
			trekRenderer.getSwapChainRenderPass(),
			globalDescriptorSetLayout->GetDescriptorSetLayout(),
			vertexShaderPath,
			fragmentShaderPath );
	}
}