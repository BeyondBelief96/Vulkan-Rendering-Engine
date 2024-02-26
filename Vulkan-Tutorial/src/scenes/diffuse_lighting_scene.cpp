#include "scene.h"

//std
#include <chrono>

namespace Trek
{
	DiffuseLightingScene::~DiffuseLightingScene()
	{
		cleanup();
	}

	void DiffuseLightingScene::setup()
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

		gameObjects.emplace(flatVase.getId(), std::move(flatVase));
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));
		gameObjects.emplace(floor.getId(), std::move(floor));

		camera.setViewTarget(glm::vec3(2.f, -1.f, -1.f), glm::vec3(0.f, 0.f, 2.5f));
		viewerObject.transform2d.translation.z = -2.5f;
	}

	void DiffuseLightingScene::render()
	{
		// Render loop
		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!trekWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			const float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(trekWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform2d.translation, viewerObject.transform2d.rotation);
			const float aspect = trekRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.f);
			if (const auto commandBuffer = trekRenderer.beginFrame())
			{
				int frameIndex = trekRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				trekRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem->renderGameObjects(frameInfo);
				trekRenderer.endSwapChainRenderPass(commandBuffer);
				trekRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(trekDevice.device());
	}

	void DiffuseLightingScene::cleanup()
	{

	}
}