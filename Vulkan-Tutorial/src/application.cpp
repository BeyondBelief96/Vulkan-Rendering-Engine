#include "application.h"
#include "simple_render_system.h"
#include "keyboard_movement_controller.h"
#include "trek_buffer.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <chrono>

namespace Trek
{
	Application::Application()
	{
		globalPool = TrekDescriptorPool::Builder(trekDevice)
			.setMaxSets(TrekSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, TrekSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	void Application::run()
	{
		// Setting up global uniform buffers.
		std::vector<std::unique_ptr<TrekBuffer>> uboBuffers(TrekSwapChain::MAX_FRAMES_IN_FLIGHT);
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

		auto globalDescriptorSetLayout = TrekDescriptorSetLayout::Builder(trekDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(TrekSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			TrekDescriptorWriter(*globalDescriptorSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		// Initializing render system and camera system.
		const SimpleRenderSystem simpleRenderSystem{ 
			trekDevice, 
			trekRenderer.getSwapChainRenderPass(),
			globalDescriptorSetLayout->GetDescriptorSetLayout(),
			"shaders/directional_lighting_ubo_vertex.spv",
			"shaders/directional_lighting_ubo_fragment.spv"
		};

		TrekCamera camera{};
		camera.setViewTarget(glm::vec3(2.f, -1.f, -1.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = TrekGameObject::createGameObject();
		viewerObject.transform2d.translation.z = -2.5f;
		const KeyboardMovementController cameraController{};

		// Render loop
		auto currentTime = std::chrono::high_resolution_clock::now();
		while(!trekWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			const float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(trekWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform2d.translation, viewerObject.transform2d.rotation);
			const float aspect = trekRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.f);
			if(const auto commandBuffer = trekRenderer.beginFrame())
			{
				int frameIndex = trekRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				trekRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				trekRenderer.endSwapChainRenderPass(commandBuffer);
				trekRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(trekDevice.device());
	}

	void Application::loadGameObjects()
	{
		const std::shared_ptr<TrekModel> flatVaseModel = TrekModel::createModelFromFile(trekDevice, "models/flat_vase.obj");
		const std::shared_ptr<TrekModel> smoothVaseModel = TrekModel::createModelFromFile(trekDevice, "models/smooth_vase.obj");
		const std::shared_ptr<TrekModel> floorModel = TrekModel::createModelFromFile(trekDevice, "models/quad.obj");

		auto flatVase = TrekGameObject::createGameObject();
		flatVase.model = flatVaseModel;
		flatVase.transform2d.translation = { -.5f, .5f, 0.f };
		flatVase.transform2d.scale = glm::vec3{ 3.f };

		auto smoothVase = TrekGameObject::createGameObject();
		smoothVase.model = smoothVaseModel;
		smoothVase.transform2d.translation = { .5f, .5f, 0.f };
		smoothVase.transform2d.scale = glm::vec3{ 3.f };

		auto floor = TrekGameObject::createGameObject();
		floor.model = floorModel;
		floor.transform2d.translation = { 0.f, .5f, 0.f };
		floor.transform2d.scale = { 3.f, 1.f, 3.f };

		gameObjects.push_back(std::move(flatVase));
		gameObjects.push_back(std::move(smoothVase));
		gameObjects.push_back(std::move(floor));
	}
}

